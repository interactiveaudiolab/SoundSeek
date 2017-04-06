//
//  main.cpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/13/16.
//
#define CLI

#include <stdio.h>
#include <chrono>
#include "../src/FeatureExtractor.h"
#include "../src/libs/json.hpp"
#include "../src/AudioSearchEngine.h"
#include "../src/FileUtils.h"
#include "../src/AudioSearchEngine2.h"

typedef std::chrono::high_resolution_clock Clock;

using namespace boost::filesystem;
using namespace std;
using namespace essentia;
using nlohmann::json;

int main (int argc, const char* argv[])
{
    // int result = Catch::Session ().run (argc, argv);
    // DBG (result);
    // Distance::testDTW ();
    // insert code here...
    path dirPath = argv[1];
    path queryPath = argv[2];
    AudioSearchEngine engine;

    auto start = Clock::now ();
    engine.addSearchDir (dirPath, true);
    // engine.removeAllAnalysisFiles ();
    vector<double> weights (AudioFeatures::num_features, .1);
    engine.search (queryPath, .1);
    engine.printFeatureInfo ();

    auto topResults = engine.getTopResults ();
    auto resultsByFeature = engine.getNearestResultByFeature ();
    // engine.addSearchDir (filePath, true);
    auto end = Clock::now ();

    cout << "\n\n########################################" << endl;
    cout << "Query: \n+\t" << queryPath.filename () << endl;
    cout << "Top results: " << endl;
    for (int i = 0; i < std::min (10, (int) topResults.size ()); ++i)
    {
        cout << "+\t" << topResults[i].toString () << endl;
    }
    cout << "\nBest results by feature:" << endl;
    for (int i = 0; i < AudioFeatures::num_features; ++i)
    {
        cout << "+ " << AudioFeatures::getAllFeatures ()[i] << " \t" << resultsByFeature[i].toString () << endl;
    }
    cout << "########################################" << endl;

    cout << "\n\nSearched in: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count () / 1000.f
         << "s" << endl;

    system (string ("open " + queryPath.string ()).c_str ());

    system (string ("open " + topResults[0].p.string ()).c_str ());
    return 0;
}
