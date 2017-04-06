//
//  AudioSearchEngine.cpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/21/16.
//

#include "AudioSearchEngine.h"

AudioSearchEngine::AudioSearchEngine () : search_dirs (0), dtw ()
{
    num_processors = std::thread::hardware_concurrency ();
    DBG (num_processors << " Processors");
    extractor.init (4096, 4096);
}

AudioSearchEngine::~AudioSearchEngine ()
{
}

void AudioSearchEngine::search (path query_path, double dtw_constraint)
{
    assert (search_dirs.size () > 0);
    assert (dtw_constraint <= 1 && dtw_constraint > 0);
    DBG ("Query: " << query_path.string () << endl);
    AudioObject query;

    // Check validity of query_path, and analyze if necessary
    if (FileUtils::existsAsAudioFile (query_path))
    {
        if (!FileUtils::analysisFileExists (query_path))
        {
            extractor.extract (query_path, true);
        }
        query = AudioObject (FileUtils::getAnalysisFilePath (query_path));
    }
    else if (FileUtils::isAnalysisFile (query_path))
    {
        query = AudioObject (query_path);
    }
    else
    {
        DBG ("Query does not exist.");
        return;
    }

    assert (analysis_files.size () > 0);

    double best_dist = std::numeric_limits<double>::max ();  // current lowest distance

    distances = MatrixXd::Zero ((int) AudioFeatures::num_features, analysis_files.size ());

    double dist;
    DBG ("Computing DTW for " << analysis_files.size () << " files." << endl);

#ifdef CLI
    boost::progress_display show_progress (analysis_files.size ());
#endif
    for (int i = 0; i < analysis_files.size (); ++i)
    {
        auto afile = analysis_files[i];

        // don't include query in search results
        if (query_path == FileUtils::getAudioFromAnalysisFile (afile)) continue;

        // DBG (afile.filename ().string ());
        AudioObject candidate = FileUtils::loadAnalysisFile (afile);

        auto result = Distance::distance (query, candidate, dtw_constraint, num_processors);
        for (int j = 0; j < result.size (); ++j)
        {
            distances (j, i) = result[j];
        }
#ifdef CLI
        ++show_progress;
#endif
    }
    removeNanInf (&distances);
    normalize (&distances, false);

    DBG ("Rows: " << distances.rows () << " Cols: " << distances.cols () << endl);
}

vector<searchResult> AudioSearchEngine::getNearestResultByFeature () const
{
    assert (analysis_files.size () > 0);
    assert (distances.rows () == AudioFeatures::num_features);

    vector<searchResult> nearestByFeature (AudioFeatures::num_features);
    vector<int> indices (AudioFeatures::num_features);
    for (int i = 0; i < AudioFeatures::num_features; ++i)
    {
        nearestByFeature[i].score = distances.row (i).minCoeff (&indices[i]);
        nearestByFeature[i].p = FileUtils::getAudioFromAnalysisFile (analysis_files[indices[i]]);
    }
    return nearestByFeature;
}

vector<searchResult> AudioSearchEngine::getTopResults (int numResults, int p, bool remove_zeros) const
{
    assert (analysis_files.size () > 0);
    assert (distances.rows () == AudioFeatures::num_features);

    vector<double> weights (AudioFeatures::num_features, 1);

    auto weighted_dists = weightedLPNorm (distances, weights, analysis_files, p);
    std::sort (weighted_dists.begin (), weighted_dists.end (), compare);

    if (remove_zeros)
    {
        for (vector<searchResult>::iterator it = weighted_dists.begin (); it < weighted_dists.end (); ++it)
        {
            if (it->score == 0)
            {
                weighted_dists.erase (it);
            }
        }
    }

    return vector<searchResult> (weighted_dists.begin (), weighted_dists.begin () + numResults);
}

vector<searchResult> AudioSearchEngine::getTopResultsWeighted (const vector<double> &weights, int p,
                                                               bool remove_zeros) const
{
    assert (analysis_files.size () > 0);
    assert (distances.rows () == AudioFeatures::num_features);

    auto weighted_dists = weightedLPNorm (distances, weights, analysis_files, p);
    std::sort (weighted_dists.begin (), weighted_dists.end (), compare);
    if (remove_zeros)
    {
        for (vector<searchResult>::iterator it = weighted_dists.begin (); it < weighted_dists.end (); ++it)
        {
            if (it->score == 0)
            {
                weighted_dists.erase (it);
            }
        }
    }

    return weighted_dists;
}

void AudioSearchEngine::addSearchDir (path dir, bool include_subdirs)
{
    if (!is_directory (dir)) return;

    for (vector<pair<path, bool>>::iterator it = search_dirs.begin (); it != search_dirs.end ();)
    {
        // check if there is already a search directory that contains the new directory
        if (it->second && dir.string ().find (it->first.string ()) != string::npos) return;

        // if the new directory contains a directory that has already been added, remove the redundant directory
        if (include_subdirs && it->first.string ().find (dir.string ()) != string::npos)
            it = search_dirs.erase (it);
        else
            ++it;
    }

    createAnalysisFiles (dir, include_subdirs);
    FileUtils::getAllAnalysisFiles (dir, analysis_files, include_subdirs);
    search_dirs.push_back (make_pair (dir, include_subdirs));
}

void AudioSearchEngine::createAnalysisFiles (path p, bool recursive) const
{
    if (is_directory (p))
    {
        directory_iterator end_itr;
        for (directory_iterator itr (p); itr != end_itr; ++itr)
        {
            // if subdirectory found, create analysis files for contents
            if (is_directory (itr->status ()) && recursive)
            {
                createAnalysisFiles (itr->path (), true);
            }

            // check if file is an audio file and if it already has an analysis file
            else if (FileUtils::existsAsAudioFile (itr->path ()) && !FileUtils::analysisFileExists (itr->path ()))
            {
                extractor.extract (itr->path (), true);
            }
        }
    }

    else if (FileUtils::existsAsAudioFile (p) && !FileUtils::analysisFileExists (p))
    {
        extractor.extract (p, true);
    }
}

void AudioSearchEngine::removeAllAnalysisFiles () const
{
    for (auto d : search_dirs) FileUtils::removeAnalysisFiles (d.first);
}

size_t AudioSearchEngine::getNumSearchFiles () const
{
    return analysis_files.size ();
}

void AudioSearchEngine::printFeatureInfo () const
{
    for (int i = 0; i < distances.rows (); ++i)
    {
        cout << AudioFeatures::getAllFeatures ()[i] << "\nMin: " << distances.row (i).minCoeff ()
             << "\nMax: " << distances.row (i).maxCoeff () << endl
             << endl;
    }
}

void AudioSearchEngine::calc_all_distances ()
{
    // for
    distances_calculated = true;
    for (int i = 0; i < analysis_files.size()
}

vector<double> AudioSearchEngine::calc_distance (path p1, path p2) const
{
    if (distances_calculated)
    {
    }
    else
    {
        return Distance::distance (AudioObject (p1), AudioObject (p2), .1, num_processors);
    }
    return vector<double> ();
}

/* PRIVATE */

void AudioSearchEngine::normalize (MatrixXd *distances_by_feature, bool use_std_Dev)
{
    // DBG ("MaxCoeff: " << distances_by_feature->row (0).maxCoeff () << endl);
    DBG ("Before Normalization:\n" << distances_by_feature->row (0) << endl);

    if (use_std_Dev)
    {
        for (int i = 0; i < distances_by_feature->rows (); ++i)
        {
            double mean = distances_by_feature->row (i).mean ();
            double accumulator = 0;
            double size = distances_by_feature->row (i).size ();
            for (int j = 0; j < size; ++j)
            {
                accumulator += pow (distances_by_feature->row (i) (j) - mean, 2);
            }
            accumulator /= size;
            double std_dev = std::sqrt (accumulator);
            double scalar = 1 / std_dev;
            for (int j = 0; j < distances_by_feature->row (i).size (); ++j)
            {
                distances_by_feature->operator() (i, j) -= mean;
            }
            distances_by_feature->row (i) *= scalar;
        }
    }
    else
    {
        for (int i = 0; i < distances_by_feature->rows (); ++i)
        {
            // distances_by_feature->row(i).normalize();
            distances_by_feature->row (i) /= (double) distances_by_feature->row (i).maxCoeff ();
        }
    }

    // DBG ("Normalized:\n" << distances_by_feature->row (0) << endl);
}

void AudioSearchEngine::removeNanInf (MatrixXd *m)
{
    double max_val = std::numeric_limits<double>::max ();

    for (int i = 0; i < m->rows (); ++i)
    {
        for (int j = 0; j < m->row (i).size (); ++j)
        {
            if (m->operator() (i, j) == INFINITY)
            {
                m->operator() (i, j) = max_val;
            }
            else if (isnan (m->operator() (i, j)))
            {
                m->operator() (i, j) = 0;
            }
        }
    }
}

bool AudioSearchEngine::compare (searchResult a, searchResult b)
{
    return a.score < b.score;
}

vector<path> AudioSearchEngine::pathsFromPairs (const vector<pair<path, double>> &pairs)
{
    vector<path> result (pairs.size ());
    for (int i = 0; i < pairs.size (); ++i) result[i] = pairs[i].first;
    return result;
}

template <typename T>
vector<searchResult> AudioSearchEngine::weightedLPNorm (const MatrixXd &m, const vector<T> &weights,
                                                        const vector<path> &paths, int p)
{
    assert (weights.size () == m.rows ());
    // DBG ("WeightedDistance");
    vector<searchResult> result (m.cols ());
    // loop over columns (instances)
    for (int i = 0; i < m.cols (); ++i)
    {
        double dist = 0;
        // loop over rows (features)
        for (int j = 0; j < m.rows (); ++j)
        {
            // DBG (m (j, i));
            dist += weights[i] * pow (m (j, i), p);
        }
        result[i].p = FileUtils::getAudioFromAnalysisFile (paths[i]);
        result[i].score = pow (dist, 1 / (double) p);
    }
    // DBG (result);
    return result;
}
