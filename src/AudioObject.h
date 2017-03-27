//
//  AudioObject.hpp
//  AudioSearchCLI
//
//  Created by Michael on 12/13/16.
//  Copyright © 2016 Michael Donovan. All rights reserved.
//

#ifndef AudioObject_hpp
#define AudioObject_hpp

#include <aquila.h>
#include "Common.h"
#include "AudioFeatures.h"

#define NUM_FEATURES 19

using namespace std;
using namespace essentia;
using namespace Aquila;
using nlohmann::json;

/**
 *  A json dictionary containing a set of features representing an audio file
 */
class AudioObject
{
public:
    AudioObject ()
    {
    }

    AudioObject (boost::filesystem::path p)
    {
        ifstream afile_stream (p.string (), std::ifstream::in);

        if (afile_stream.is_open ()) afile_stream >> features;
    }

    ~AudioObject ()
    {
    }

    DtwDataType dtwDataType () const
    {
        DtwDataType result (0);
        for (auto feature : AudioFeatures::getAllFeatures ())
        {
            /*
            DtwDataType temp (0);
            temp.push_back (features[feature]);
            result.push_back (temp);
             */
            result.push_back (features[feature]);
        }
        return result;
    }

    size_t getNumSamples () const
    {
        return features["pitch"].size ();
    }

    json features;

private:
};

#endif /* AudioObject_hpp */
