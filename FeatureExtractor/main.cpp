#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/progress.hpp>
#include "../src/FileUtils.h"
#include "../src/FeatureExtractor.h"
#include <ctime>
#include <vector>

namespace  fs = boost::filesystem;

int main(int argc, char* argv[]) {
    fs::path path_to_audio;
    try{
        path_to_audio = argv[1];
    }
    catch(...){
        std::cout << "Usage: FeatureExtractor <audio_dir>" << std::endl;
        return 0;
    }

    int num_files = 0;
    for (fs::recursive_directory_iterator it (path_to_audio); it != fs::recursive_directory_iterator (); ++it)
    {
        if(FileUtils::existsAsAudioFile(it->path()))
            num_files++;
    }

    FeatureExtractor extractor;
    extractor.init();

    boost::progress_display show_progress (num_files);
    std::vector<float> times;
    
    for (fs::recursive_directory_iterator it (path_to_audio); it != fs::recursive_directory_iterator (); ++it)
    {
        auto start = std::clock();
        if(FileUtils::existsAsAudioFile(it->path()))
        {
            extractor.extract(it->path());
            auto end = std::clock();
            std::cout << std::endl << end-start << std::endl;
            times.push_back((end-start)/ (float)CLOCKS_PER_SEC);
        }
        ++show_progress;

    }
    float mean = std::accumulate(times.begin(), times.end(), 0.0) / (float)num_files;
    std::cout << "Mean time: " << mean << std::endl;
    return 0;
}
