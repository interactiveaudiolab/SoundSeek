//
//  AudioSearchEngine2.hpp
//  AudioSearchCLI
//
//  Created by Michael on 4/6/17.
//  Copyright © 2017 Michael Donovan. All rights reserved.
//

#ifndef AudioSearchEngine2_hpp
#define AudioSearchEngine2_hpp

#include "Common.h"
#include "FileUtils.h"
#include <boost/multi_array.hpp>

#define DEFAULT_DTW_CONSTRAINT .1

using namespace std;
using namespace boost;
using namespace boost::filesystem;

class AudioSearchEngine2
{
public:
    AudioSearchEngine2 ()
    {
        featureWeights.resize (AudioFeatures::num_features, 1);
    }

    ~AudioSearchEngine2 ()
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
        // if audio file exists, is an audio file, and is not already in database
        if (FileUtils::existsAsAudioFile (file) && !hasSound (file))
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
                addFile (it->path (), false);
            }
        }
        else
        {
            for (directory_iterator it (dir); it != directory_iterator (); ++it)
            {
                addFile (it->path (), false);
            }
        }

        resize ();  // resize once at end rather than once for every file added
    }

    /**
     *  Calculate distances between every pair of sounds in the databse
     */
    void calcAllDistances ()
    {
        for (int i = 0; i < distances.shape ()[0]; ++i)
        {
            for (int j = i; j < distances.shape ()[1]; ++j)
            {
                if (j == i)
                {
                    distances[i][j] = vector<double> (AudioFeatures::num_features, 0.);
                    continue;
                }

                calcDistance (sounds[i], sounds[j]);
            }
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
        calcDistance (a, b);  // will calculate distance if not already calculated
        return distances[pathToID (a)][pathToID (b)];
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

        return Distance::weightedPNorm (dists, featureWeights);
    }

private:
    deque<path> sounds;
    multi_array<vector<double>, 2> distances;
    vector<double> featureWeights;

    void resize ()
    {
        assert (distances.shape ()[0] == distances.shape ()[1]);
        if (distances.shape ()[0] != sounds.size ())
        {
            distances.resize (extents[sounds.size ()][sounds.size ()]);
        }
    }

    bool distCalculated (path a, path b)
    {
        auto a_id = pathToID (a);
        auto b_id = pathToID (b);

        if (distances[a_id][b_id].size () == 0) return false;
        return true;
    }

    size_t pathToID (path p)
    {
        auto it = find (sounds.begin (), sounds.end (), p);
        assert (it != sounds.end ());

        auto id = distance (sounds.begin (), it);
        return id;
    }

    void calcDistance (path a, path b)
    {
        if (distCalculated (a, b)) return;

        auto a_id = pathToID (a);
        auto b_id = pathToID (b);

        auto dists = Distance::distance (AudioObject (sounds[a_id]), AudioObject (sounds[b_id]), DEFAULT_DTW_CONSTRAINT,
                                         thread::hardware_concurrency ());
        distances[a_id][b_id] = dists;
        distances[b_id][a_id] = dists;
    }
};

#endif /* AudioSearchEngine2_hpp */
