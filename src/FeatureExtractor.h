//
//  FeatureExtractor.hpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/13/16.
//

#ifndef FeatureExtractor_hpp
#define FeatureExtractor_hpp

#include <essentia/algorithmfactory.h>
#include "AudioFeatures.h"

#define DEFAULT_FRAMESIZE 2048
#define DEFAULT_HOPSIZE 2048
#define DEFAULT_SAMPLERATE 16000

using namespace std;
using namespace essentia;
using namespace essentia::standard;
using namespace boost::filesystem;
using nlohmann::json;

/// Class for extracting features from an buffer of audio samples
class FeatureExtractor
{
public:
    FeatureExtractor ();
    ~FeatureExtractor ();

    void init (int frame_size = DEFAULT_FRAMESIZE, int hop_size = DEFAULT_HOPSIZE,
               int sample_rate = DEFAULT_SAMPLERATE);

    /**
     *  Extracts a set of features for an audio file and writes them to an analysis file
     *
     *  @param file_path           Path to an audio file
     *  @param write_analysis_file If true, write analysis. Else print features to stdout
     */
    void extract (path file_path, bool write_analysis_file = true) const;

protected:
    int frame_size, hop_size, resample_rate;
};
#endif /* FeatureExtractor_hpp */
