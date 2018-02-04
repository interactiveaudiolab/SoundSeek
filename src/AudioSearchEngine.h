//
//  AudioSearchEngine.hpp
//  AudioSearchCLI
//
//  Created by Michael on 4/6/17.
//  Copyright © 2017 Michael Donovan. All rights reserved.
//

#ifndef AudioSearchEngine_hpp
#define AudioSearchEngine_hpp

#include "Common.h"
#include "FileUtils.h"
#include "Distance.h"
#include "libs/json.hpp"
#include "Config.h"

#include <boost/progress.hpp>
#include <boost/filesystem.hpp>
#include <ctime>
#include <limits>

// serialization
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <fstream>

#define DEFAULT_DTW_CONSTRAINT .1

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using nlohmann::json;

static pair<string, string> operator, (string path1, string path2)
{
    if (path1 < path2) return make_pair (path1, path2);
    return make_pair (path2, path1);
}

static pair<string, string> operator, (path path1, path path2)
{
    if (path1.string () < path2.string ()) return make_pair (path1.string (), path2.string ());
    return make_pair (path2.string (), path1.string ());
}

class AudioSearchEngine
{
public:
    AudioSearchEngine (bool use_cache_ = true)
    {
        feature_weights.resize (AudioFeatures::num_features, 1);
        use_cache = use_cache_;
        if (use_cache)
            loadDistances ();

        try
        {
            local_align = Config::load ()["localAlign"];
        }
        catch (...)
        {
            local_align = DEFAULT_LOCAL;
        }

        new_sounds_added = false;
    }

    ~AudioSearchEngine ()
    {
        json config = Config::load ();
        config["localAlign"] = local_align;
        Config::save (config);
        if (use_cache)
            saveDistances ();
    }

    /**
     *  Return true if a sound file is already in the database
     *
     *  @param file Path to an audio file
     *
     *  @return True if a sound file is already in the database
     */
    bool hasSound (path file)
    {
        return find (sounds.begin (), sounds.end (), file) != sounds.end ();
    }

    /**
     *  Add a sound file to the database
     *
     *  @param file          Path to an audio file
     */
    void addFile (path file)  //,  bool resize_matrix = true)
    {
        if (!FileUtils::existsAsAudioFile (file))
            throw invalid_argument (file.string () + " is not a valid audio file.");

        // if audio file exists, is an audio file, and is not already in database
        if (!hasSound (file))
        {
            sounds.push_back (file);
        }

        // if (resize_matrix) resize ();
    }

    /**
     *  Add a directory of sounds to the database
     *
     *  @param dir             Path to a directory of audio files
     *  @param include_subdirs If true, sounds in subdirectories will be added as well
     */
    void addDirectory (path dir, bool include_subdirs)
    {
        if (!is_directory (dir)) addFile (dir);

        if (include_subdirs)
        {
            for (recursive_directory_iterator it (dir); it != recursive_directory_iterator (); ++it)
            {
                if (FileUtils::existsAsAudioFile (it->path ())) addFile (it->path ());
            }
        }
        else
        {
            for (directory_iterator it (dir); it != directory_iterator (); ++it)
            {
                if (FileUtils::existsAsAudioFile (it->path ())) addFile (it->path ());
            }
        }
        new_sounds_added = true;
    }

    /**
     *  Calculate distances between every pair of sounds in the databse
     */
    void calcAllDistances ()
    {
        DBG ("Calculating all distances.");

#ifdef DEBUG
        progress_display show_progress (sounds.size ());
#endif
        clock_t start = clock ();
        for (int i = 0; i < sounds.size (); ++i)
        {
            for (int j = i; j < sounds.size (); ++j)
            {
                if (distances[sounds[i], sounds[j]].size () > 0) continue;
                if (j == i)
                    distances[sounds[i], sounds[j]] = vector<double> (AudioFeatures::num_features, 0.);
                else
                    getFeatureDistances (sounds[i], sounds[j]);
            }
#ifdef DEBUG
            ++show_progress;
#endif
        }
        double duration = (clock () - start) / (double) CLOCKS_PER_SEC;
        cerr << "Precalculated distances for " << sounds.size () << " sounds in " << duration << " seconds" << endl;
        if (use_cache)
            saveDistances ();
    }

    /**
     *  Delete any cached distance values, recalculate all.
     */
    void reCalcAllDistances ()
    {
        distances.clear ();
        calcAllDistances ();
    }

    /**
     *  Get vector of distances by feature between two sounds
     *
     *  @param a Path to an audio file
     *  @param b Path to an audio file
     *
     *  @return vector<double> containing distances along each feature
     */
    vector<double> getFeatureDistances (path a, path b)
    {
        if (distCalculated (a, b)) return distances[a, b];

        auto dists = Distance::distance (AudioObject (a), AudioObject (b), thread::hardware_concurrency (), local_align);
        distances[a, b] = dists;
        distances[a, b] = dists;

        return dists;
    }

    /**
     *  Get the weighted distance between two sounds
     *
     *  @param a Path to an audio file
     *  @param b Path to an audio file
     *
     *  @return The weighted distance between two sounds
     */
    double getWeightedDistance (path a, path b)
    {
        auto dists = getFeatureDistances (a, b);
        return Distance::weightedPNorm (dists, feature_weights);
        // return Distance::weightedPNorm<double> (dists, featureWeights);
    }

    /**
     *  Get the sounds that are nearest to a given query along each feature axis
     *
     *  @param query Path to an audio file
     *
     *  @return vector<path> of the nearest sounds along each feature axis
     */
    vector<path> getNearestByFeature (path query)
    {
        addFile (query);

        // auto query_id = pathToID (query);

        vector<path> result (AudioFeatures::num_features);
        clock_t start = clock ();
        for (int feature = 0; feature < AudioFeatures::num_features; ++feature)
        {
            double min_dist = numeric_limits<double>::max ();
            path min_sound;

            for (int sound = 0; sound < sounds.size (); ++sound)
            {
                auto dist = getFeatureDistances (query, sounds[sound])[feature];
                if (sounds[sound] != query && dist > 0 && dist < min_dist)
                {
                    min_dist = dist;
                    min_sound = sounds[sound];
                }
            }
            result[feature] = min_sound;
        }
        double duration = (clock () - start) / (double) CLOCKS_PER_SEC;
        cerr << "Searched " << sounds.size () << " sounds in " << duration << " seconds" << endl;

        return result;
    }

    /**
     *  Get a ranked like of sounds that are nearest to a given query using weighted euclidean distance
     *  Features are equally weighted by default, can be reweighted using addLikedSound.
     *
     *  @param query       Path to an audio file
     *  @param num_results The number of results to return
     *
     *  @return ranked list of nearest sounds
     */
    vector<path> getNearestWeighted (path query, int num_results = 5)
    {
        addFile (query);

        // auto query_id = pathToID (query);
        clock_t start = clock ();
        vector<double> dists (sounds.size ());
        for (int i = 0; i < sounds.size (); ++i)
        {
            if (sounds[i] == query)
                dists[i] = numeric_limits<double>::max ();
            else
                dists[i] = getWeightedDistance (query, sounds[i]);
        }

        // argsort
        vector<int> indices (sounds.size ());
        iota (indices.begin (), indices.end (), 0);

        sort (indices.begin (), indices.end (), [&](int a, int b) { return dists[a] < dists[b]; });

        indices.pop_back ();

        vector<path> results;

        for (int i = 0; i < std::min(num_results, (int)sounds.size()); ++i)
        {
            if (sounds[indices[i]] != query) results.push_back (sounds[indices[i]]);
        }
        double duration = (clock () - start) / (double) CLOCKS_PER_SEC;
        cerr << "Searched " << sounds.size () << " sounds in " << duration << " seconds" << endl;

        return results;
    }

    /**
     *  Mark a sound as "liked" to reweight features. Takes a sound and the query sound and adds weight to the feature
     * axis on which they are closest.
     *
     *  @param likedSound path to an audio file
     *  @param query path to an audio file
     */
    void addLikedSound (path likedSound, path query)
    {
        // very naive feature reweighting. find feature axis along which the two sounds are closest and weight that
        // feature higher
        auto dists = distances[query, likedSound];
        auto nearest_feature_index = min_element (dists.begin (), dists.end ()) - dists.begin ();
        feature_weights[nearest_feature_index] *= 1.1;
        cerr << feature_weights << endl;
    }

    void setUseLocalAlign (bool useLocal)
    {
        cerr << "Local: " << useLocal << endl;
        local_align = useLocal;
    }

    bool getUseLocal ()
    {
        return local_align;
    }

private:
    deque<path> sounds;
    // multi_array<vector<double>, 2> distances;
    map<pair<string, string>, vector<double>> distances;
    vector<double> feature_weights;
    bool new_sounds_added, local_align, use_cache;

    /**
     *  Return true if the SearchEngine has already calculated a distance for a pair of sounds
     *
     *  @param a path to an audio file
     *  @param b path to an audio file
     *
     *  @return true if the SearchEngine has already calculated a distance for a pair of sounds
     */
    bool distCalculated (path a, path b)
    {
        if (distances[a, b].size () == 0) return false;
        return true;
    }

    void printDists ()
    {
        //        DBG ("distances.shape: " << distances.shape ()[0] << ", " << distances.shape ()[1]);
        //
        //        for (int i = 0; i < distances.shape ()[0]; ++i)
        //        {
        //            for (int j = 0; j < distances.shape ()[1]; ++j)
        //            {
        //                DBG ("i: " << i << " j: " << j);
        //                cerr << (distances[i][j].size () > 0);
        //            }
        //            cerr << endl << endl;
        //        }
    }

    void saveDistances ()
    {
        if (!new_sounds_added) return;
        try
        {
            path cache_dir (string (getenv ("HOME")) + string (APP_SUPPORT) + string (DISTANCE_CACHE_PATH));
            cache_dir = cache_dir.parent_path ();
            if (!is_directory (cache_dir)) create_directory (cache_dir);
            std::ofstream file (string (getenv ("HOME")) + string (APP_SUPPORT) + string (DISTANCE_CACHE_PATH));
            archive::text_oarchive ar (file);
            boost::serialization::serialize (ar, distances, 1);
            cerr << "Saving distance cache..." << endl;
        }
        catch (...)
        {
            cerr << "Writing distance cache failed" << endl;
        }
    }

    void loadDistances ()
    {
        try
        {
            std::ifstream file (string (getenv ("HOME")) + string (APP_SUPPORT) + string (DISTANCE_CACHE_PATH));
            archive::text_iarchive ar (file);
            boost::serialization::serialize (ar, distances, 1);
            cerr << "Loading distance cache..." << endl;
        }
        catch (...)
        {
            cerr << "Loading distance cache failed" << endl;
        }
    }
};

#endif /* AudioSearchEngine_hpp */
