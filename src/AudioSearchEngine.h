//
//  AudioSearchEngine.hpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/21/16.
//

#ifndef AudioSearchEngine_hpp
#define AudioSearchEngine_hpp

#include <cassert>
#include <sstream>
#include <thread>
#include <boost/progress.hpp>
#include <boost/timer.hpp>
#include <limits>
#include <aquila.h>
#include "FeatureExtractor.h"
#include "FileUtils.h"
#include "AudioFeatures.h"
#include "Common.h"
#include "Distance.h"

using namespace std;
// using namespace fastdtw;
using namespace Aquila;
using namespace Eigen;

struct searchResult
{
    path p;
    double score;

    /**
     *  Creates a string representation of a searchResult
     *
     *  @return a string representation of a searchResult
     */
    string toString ()
    {
        stringstream ss;
        ss << scientific << setprecision (3) << score << "\t" << p.filename ().string ();
        return ss.str ();
    }
};

class AudioSearchEngine
{
public:
    AudioSearchEngine ();

    ~AudioSearchEngine ();

    /**
     *  Finds the audio file in search_dirs most similar to a given audio file
     *
     *  @param query_path The path to a query audio file
     *  @param dtw_constraint A locality constraint for DTW as a fraction of the size of the time series being compared
     */
    void search (path query_path, double dtw_constraint = 1);

    /**
     *  Return the nearest neighbor by feature
     *  Must call search() fist
     *
     *  @return vector<searchResult>
     */
    vector<searchResult> getNearestResultByFeature () const;

    /**
     *  Return all search results sorted by distance
     *  Must call search() fist
     *
     *  @param p P for LP norm
     *
     *  @return vector<searchResult> of sorted results.
     */
    vector<searchResult> getTopResults (int num_results = 10, int p = 2, bool remove_zeros = true) const;

    /**
     *  Returns all search result sorted by weighted distance
     *  Must call search() fist
     *
     *  @param weights vector of weights for each feature
     *  @param p       P for LP norm
     *
     *  @return vector<searchResult> of sorted results.
     */
    vector<searchResult> getTopResultsWeighted (const vector<double> &weights, int p = 2,
                                                bool remove_zeros = true) const;

    /**
     *  Add a directory to be searched
     *
     *  @param dir             The directory to be searched
     *  @param include_subdirs If true, also search subdirectories
     */
    void addSearchDir (path dir, bool include_subdirs);

    /**
     *  Creates analysis files for an audio file or directory of audio files
     *
     *  @param p            A path to an audio file or directory of audio files
     *  @param recursive    If true and p is a directory, create analysis files for files in subdirectories
     */
    void createAnalysisFiles (path p, bool recursive = true) const;

    /**
     *  Deletes all existing analysis files in search_dirs
     */
    void removeAllAnalysisFiles () const;

    /**
     *  Get the number of audio files in the set being searched
     *
     *  @return The number of audio files in the set being searchd
     */
    size_t getNumSearchFiles () const;

    /* For debugging */
    void printFeatureInfo () const;

private:
    FeatureExtractor extractor;
    Dtw dtw;
    MatrixXd distances;
    vector<path> analysis_files;
    vector<pair<path, bool>> search_dirs;  // The paths to the directories to be searched, and bools where True =
    // directory should be searched recursively
    unsigned int num_processors;

    void normalize (MatrixXd *distances_by_feature, bool use_std_dev = false);

    /**
     *  Replace occurences of infinity with double max, replace nans with 0
     *
     *  @param m MatrixXd
     */
    void removeNanInf (MatrixXd *m);

    /**
     *  Comparison function for sorting a vector<searchResult>
     */
    static bool compare (searchResult a, searchResult b);

    /**
     *  Extracts paths from a vector<pair<path, double>>
     */
    vector<path> pathsFromPairs (const vector<pair<path, double>> &pairs);

    /**
     *  Compute weighted LP norm for multiple instances with precomputed distances along individual axes
     *
     *  @param m       2d matrix, rows = feature dimensions, cols = instances (audio files). [row,col] = DTW distance
     *  between instance and query for one feature
     *  @param weights vector of weights for each feature
     *  @param paths   vector of paths to audio files corresponding to instances
     *  @param p
     *
     *  @return A vector of structs containing the paths to the audio files and their respective distances
     */
    template <typename T>
    static vector<searchResult> weightedLPNorm (const MatrixXd &m, const vector<T> &weights, const vector<path> &paths,
                                                int p = 2);
};

#endif /* AudioSearchEngine_hpp */
