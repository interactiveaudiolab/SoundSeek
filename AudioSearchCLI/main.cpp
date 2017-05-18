//
//  main.cpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/13/16.
//
#define CLI

#define CATCH_CONFIG_RUNNER

#include "../src/libs/catch.hpp"

#include <stdio.h>
#include <chrono>
#include "../src/libs/json.hpp"
//#include "../src/AudioSearchEngine.h"
#include "../src/FileUtils.h"
#include "../src/AudioSearchEngine.h"
#include "../src/tests.h"

#define CACHE_PATH "/Users/michael/InteractiveAudioLab/SoundSeek/caches/distcache.ar"

typedef std::chrono::high_resolution_clock Clock;

using namespace boost::filesystem;
using namespace std;
using namespace essentia;
using nlohmann::json;

int main (int argc, const char *argv[])
{
    //    int result = Catch::Session ().run (argc, argv);
    Test::run_tests ();
    // DBG (result);
    // Distance::testDTW ();
    // insert code here...
    path dirPath = argv[1];
    path queryPath = argv[2];

    auto start = Clock::now ();

    AudioSearchEngine engine;
    engine.addDirectory (dirPath, true);
    engine.reCalcAllDistances ();

    // engine.addSearchDir (dirPath, true);
    // engine.removeAllAnalysisFiles ();
    // vector<double> weights (AudioFeatures::num_features, .1);
    // engine.search (queryPath, .1);
    // engine.printFeatureInfo ();

    auto results = engine.getNearestWeighted (queryPath);

    //    auto topResults = engine.getTopResults ();
    auto resultsByFeature = engine.getNearestByFeature (queryPath);
    // engine.addSearchDir (filePath, true);
    auto end = Clock::now ();

    cout << "\n\n########################################" << endl;
    cout << "Query: \n+\t" << queryPath.filename () << endl;
    cout << "Top results: " << endl;
    for (int i = 0; i < std::min (10, (int) results.size ()); ++i)
    {
        try
        {
            cout << "+\t" << results[i] << endl;
            cout << " " << engine.getWeightedDistance (queryPath, results[i]) << endl;
        }
        catch (...)
        {
        }
    }

    cout << "\nBest results by feature:" << endl;
    for (int i = 0; i < AudioFeatures::num_features; ++i)
    {
        try
        {
            cout << "+ " << AudioFeatures::getAllFeatures ()[i] << " \t" << resultsByFeature[i].string () << endl;
            cout << " " << engine.getFeatureDistances (queryPath, resultsByFeature[i])[i] << endl;
        }
        catch (...)
        {
        }
    }
    cout << "########################################" << endl;

    cout << "\n\nSearched in: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count () / 1000.f
         << "s" << endl;

    //    std::system (string ("open " + queryPath.string ()).c_str ());
    //
    //    std::system (string ("open " + results[0].string ()).c_str ());
    //    return (result < 0xff ? result : 0xff);
    return 0;
}
