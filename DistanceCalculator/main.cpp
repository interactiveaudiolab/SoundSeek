//
//  main.cpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/13/16.
//
#define CLI

#define CATCH_CONFIG_RUNNER

#include "../src/libs/catch.hpp"
#include "../src/libs/json.hpp"
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <vector>
//#include "../src/AudioSearchEngine.h"
#include "../src/AudioSearchEngine.h"
#include "../src/FeatureExtractor.h"
#include "../src/FileUtils.h"
#include "../src/tests.h"
#include "ezETAProgressBar.hpp"

#define CACHE_PATH "/Users/michael/InteractiveAudioLab/SoundSeek/caches/distcache.ar"

typedef std::chrono::high_resolution_clock Clock;

using namespace boost::filesystem;
using namespace std;
using namespace essentia;
using nlohmann::json;

std::string makeKey(path p1, path p2)
{
    if (p1.string() < p2.string()) {
        return std::string(p1.string() + "#%#%#" + p2.string());
    } else {
        return std::string(p2.string() + "#%#%#" + p1.string());
    }
}

int main(int argc, const char* argv[])
{
    //    int result = Catch::Session ().run (argc, argv);
    //    Test::run_tests();
    // DBG (result);
    // Distance::testDTW ();
    // insert code here...
    path dirPath = argv[1];

    auto start = Clock::now();
    FeatureExtractor extractor;
    extractor.init();
    AudioSearchEngine engine(false);
    std::vector<path> audioFiles;
    engine.addDirectory(dirPath, true);
    for (recursive_directory_iterator it(dirPath); it != recursive_directory_iterator(); ++it) {
        if (FileUtils::existsAsAudioFile(it->path())) {
            audioFiles.push_back(it->path());
            if (!boost::filesystem::exists(FileUtils::getAnalysisFilePath(it->path())))
                extractor.extract(it->path());
        }
    }
    ez::ezETAProgressBar bar(audioFiles.size());
    bar.start();
    json distances;
    int i = 0;
    for (auto it1 = audioFiles.begin(); it1 != audioFiles.end(); ++it1, ++bar) {
        for (auto it2 = audioFiles.begin(); it2 != audioFiles.end(); ++it2) {
            path f1 = *it1;
            path f2 = *it2;
            std::string key = makeKey(f1, f2);
            try {
                distances.at(key);
            } catch (...) {
                //                std::cout << f1.string() << ", " << f2.string() << std::endl;
                distances[key] = Distance::distance(AudioObject(f1), AudioObject(f2), 8, false, .1);
            }
        }
    }
    std::ofstream out("synthassis_distances.json");
    out << std::setw(4) << distances << std::endl;
    return 0;
}
