//
//  FileUtils.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/21/16.
//

#ifndef FileUtils_h
#define FileUtils_h

#include <boost/filesystem.hpp>
#define AFILE_EXT ".afr"

using namespace std;
using namespace boost::filesystem;

/// Static class with some useful file handling functions
namespace FileUtils
{
/**
 *  Checks whether a file exists is an audio file
 *
 *  @param p file path to check
 *
 *  @return true if file exists and is audio file
 */
static bool existsAsAudioFile (path p)
{
    if (!is_regular_file (p)) return false;

    string ext = p.extension ().string ();

    vector<string> audio_extensions = {".wav", ".mp3", ".aif", ".m4a", ".flac", ".ogg", ".caf"};

    for (auto e : audio_extensions)
    {
        if (e == ext) return true;
    }

    return false;
}

/**
 *  Check if a path is a path to an analysis file
 *
 *  @param p the path
 *
 *  @return True if p is a path to an analysis file
 */
static bool isAnalysisFile (path p)
{
    return exists (p) && p.extension ().string () == AFILE_EXT;
}

/**
 *  Given the path to an analysis file, returns the path to the corresponding audio file
 *
 *  @param path_to_afile
 *
 *  @return path to an audio file
 */
static path getAudioFromAnalysisFile (path path_to_afile)
{
    if (!isAnalysisFile (path_to_afile)) return path_to_afile;

    string p = path_to_afile.string ();
    for (int i = 0; i < 4; ++i) p.pop_back ();
    return path (p);
}

/**
 *  Returns the path to the analysis file corresponding to a given audio file
 *
 *  @param path_to_audio Path to an audio file
 *
 *  @return Path for the analysis file
 */
static path getAnalysisFilePath (path path_to_audio)
{
    return path (path_to_audio.string () + AFILE_EXT);
}

/**
 *  Check if analysis file already exists for an audio file
 *
 *  @param path_to_audio An audio file to check
 *
 *  @return True if analysis file already exists
 */
static bool analysisFileExists (path path_to_audio)
{
    return is_regular_file (getAnalysisFilePath (path_to_audio));
}

static void getAllAnalysisFiles (path dir, vector<path>& dest, bool recursive = true)
{
    directory_iterator end_itr;
    for (directory_iterator itr (dir); itr != end_itr; ++itr)
    {
        // DBG (itr->path ().string ());
        if (recursive && is_directory (itr->status ()))
        {
            getAllAnalysisFiles (dir, dest, true);
        }
        else if (isAnalysisFile (itr->path ()))
        {
            dest.push_back (itr->path ());
        }
    }
}

///**
// *  Load features from an analysis file to an AudioObject
// *
// *  @param p Path to the analysis file
// *
// *  @return An AudioObject
// */
// static path getAnalysisFile (path p)
//{
//    if (isAnalysisFile (p))
//        return p;
//    else if (existsAsAudioFile (p))
//    {
//        FeatureExtractor extractor;
//        extractor.extract (p);
//        return getAnalysisFilePath (p);
//    }
//    else
//
//        throw std::runtime_error::runtime_error (p.string () + "not a valid audio file.");
//}

/**
 *  Delete all analysis files from a given directory
 *
 *  @param dir The directory from which to remove analysis files
 */
static void removeAnalysisFiles (path dir)
{
    if (is_directory (dir))
    {
        directory_iterator end_itr;
        for (directory_iterator itr (dir); itr != end_itr; ++itr)
        {
            if (isAnalysisFile (itr->path ())) boost::filesystem::remove (itr->path ());
        }
    }
}

/**
 *  Return a vector containing valid audio file extensions
 *
 *  @return
 */
static vector<string> getValidFormats ()
{
    vector<string> formats = {".wav", ".mp3", ".aif", ".m4a", ".flac", ".ogg", ".caf"};
    return formats;
}
}

#endif /* FileUtils_h */
