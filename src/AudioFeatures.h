//
//  AudioFeatures.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 1/30/17.
//

#ifndef AudioFeatures_h
#define AudioFeatures_h

#include "Common.h"

#define NUM_MFCC 12

using namespace std;

class AudioFeatures
{
public:
    static vector<string> getAllFeatures ()
    {
        vector<string> features = {"pitch",           "pitch_confidence", "spectral_centroid", "spectral_kurtosis",
                                   "spectral_spread", "loudness"};

        for (int i = 0; i < NUM_MFCC; ++i) features.push_back ("mfcc_" + to_string (i));

        return features;
    }

    static enum featureIndices {
        pitch = 0,
        pitch_confidence,
        spectral_centroid,
        spectral_kurtosis,
        spectral_spread,
        loudness,
        mfcc_0,
        mfcc_1,
        mfcc_2,
        mfcc_3,
        mfcc_4,
        mfcc_5,
        mfcc_6,
        mfcc_7,
        mfcc_8,
        mfcc_9,
        mfcc_10,
        mfcc_11,
        num_features
    };

private:
    AudioFeatures ();
};

#endif /* AudioFeatures_h */
