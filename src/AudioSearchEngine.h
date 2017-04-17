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
#include <boost/multi_array.hpp>
#include <boost/progress.hpp>

#include <limits>

#define DEFAULT_DTW_CONSTRAINT .1

using namespace std;
using namespace boost;
using namespace boost::filesystem;

typedef multi_array<vector<double>, 2> dist_matrix;

class AudioSearchEngine
{
public:
    AudioSearchEngine ()
    {
        featureWeights.resize (AudioFeatures::num_features, 1);
    }

    ~AudioSearchEngine ()
    {
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
    void addFile (path file, bool resize_matrix = true)
    {
        if (!FileUtils::existsAsAudioFile (file))
            throw std::runtime_error (file.string () + " is not a valid audio file.");

        // if audio file exists, is an audio file, and is not already in database
        if (!hasSound (file))
        {
            sounds.push_back (file);
        }

        if (resize_matrix) resize ();
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
                if (FileUtils::existsAsAudioFile (it->path ())) addFile (it->path (), false);
            }
        }
        else
        {
            for (directory_iterator it (dir); it != directory_iterator (); ++it)
            {
                if (FileUtils::existsAsAudioFile (it->path ())) addFile (it->path (), false);
            }
        }

        resize ();  // resize once at end rather than once for every file added
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

        for (int i = 0; i < sounds.size (); ++i)
        {
            for (int j = i; j < sounds.size (); ++j)
            {
                if (distances[i][j].size () > 0) continue;
                if (j == i)
                    distances[i][j] = vector<double> (AudioFeatures::num_features, 0.);
                else
                    getFeatureDistances (sounds[i], sounds[j]);
            }
#ifdef DEBUG
            ++show_progress;
#endif
        }
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
        auto a_id = pathToID (a);
        auto b_id = pathToID (b);
        if (distCalculated (a, b)) return distances[a_id][b_id];

        auto dists = Distance::distance (AudioObject (sounds[a_id]), AudioObject (sounds[b_id]), DEFAULT_DTW_CONSTRAINT,
                                         thread::hardware_concurrency ());
        distances[a_id][b_id] = dists;
        distances[b_id][a_id] = dists;

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
        DBG ("getWeightedDistance");
        auto dists = getFeatureDistances (a, b);
        return Distance::weightedPNorm (dists, featureWeights);
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
        DBG ("getNearestByFeature");
        addFile (query);

        auto query_id = pathToID (query);

        vector<path> result (AudioFeatures::num_features);

        for (int i = 0; i < AudioFeatures::num_features; ++i)
        {
            double min_dist = numeric_limits<double>::max ();
            size_t min_id = query_id;

            for (int j = 0; j < distances.shape ()[1]; ++j)
            {
                auto dist = getFeatureDistances (query, IDToPath (j))[i];
                if (j != query_id && dist > 0 && dist < min_dist)
                {
                    min_dist = dist;
                    min_id = j;
                }
            }
            result[i] = IDToPath (min_id);
        }

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

        print_dists ();
        auto query_id = pathToID (query);

        vector<double> dists (distances.shape ()[1]);
        for (int i = 0; i < sounds.size (); ++i)
        {
            if (sounds[i] == query)
                dists[i] = numeric_limits<double>::max ();
            else
                dists[i] = getWeightedDistance (query, sounds[i]);
        }

        // argsort
        vector<int> indices (distances.shape ()[1]);
        iota (indices.begin (), indices.end (), 0);

        sort (indices.begin (), indices.end (), [&](int a, int b) { return dists[a] < dists[b]; });

        indices.pop_back ();

        vector<path> results (num_results);

        for (int i = 0; i < num_results; ++i) results[i] = sounds[indices[i]];

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
        auto dists = distances[pathToID (query)][pathToID (likedSound)];
        auto nearest_feature_index = min_element (dists.begin (), dists.end ()) - dists.begin ();
        featureWeights[nearest_feature_index] *= 1.1;
    }

private:
    deque<path> sounds;
    dist_matrix distances;
    vector<double> featureWeights;

    /**
     *  Resize the distance matrix to match the number of sounds
     */
    void resize ()
    {
        assert (distances.shape ()[0] == distances.shape ()[1]);
        if (distances.shape ()[0] != sounds.size ())
        {
            distances.resize (extents[sounds.size ()][sounds.size ()]);
        }
    }

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
        auto a_id = pathToID (a);
        auto b_id = pathToID (b);

        if (distances[a_id][b_id].size () == 0) return false;
        return true;
    }

    /**
     *  Get the index of a sound in the distance matrix
     *
     *  @param p path to an audio file
     *
     *  @return an index
     */
    size_t pathToID (path p)
    {
        auto it = find (sounds.begin (), sounds.end (), p);
        if (it == sounds.end ()) throw runtime_error ("File not in database: " + p.string ());
        return distance (sounds.begin (), it);
    }

    /**
     *  Get the path to the sound associated with a given index
     *
     *  @param ID index of the sound
     *
     *  @return path to the sound
     */
    path IDToPath (size_t ID)
    {
        return sounds[ID];
    }

    void print_dists ()
    {
        DBG ("distances.shape: " << distances.shape ()[0] << ", " << distances.shape ()[1]);

        for (int i = 0; i < distances.shape ()[0]; ++i)
        {
            for (int j = 0; j < distances.shape ()[1]; ++j)
            {
                DBG ("i: " << i << " j: " << j);
                cerr << (distances[i][j].size () > 0);
            }
            cerr << endl << endl;
        }
    }
};

#endif /* AudioSearchEngine_hpp */
