//
//  Macros.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 1/11/17.
//

#ifndef Common_h
#define Common_h

#include <cassert>
#include <math.h>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <essentia/pool.h>
#include <essentia/essentiamath.h>

#include <Eigen/Core>

#include "libs/json.hpp"

#define APP_SUPPORT "/Library/Application\ Support/SoundSeek/"
#define DISTANCE_CACHE_PATH "distance.cache"
#define CFG_PATH "soundseek.cfg"

#ifdef DEBUG
#define DBG(x) cerr << x << endl
#else
#define DBG(x) \
    do         \
    {          \
    } while (0)
#endif

#endif /* Common_h */
