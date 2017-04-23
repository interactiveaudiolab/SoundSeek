#ifndef AUDIOREADER_H
#define AUDIOREADER_H


#include <essentia/algorithmfactory.h>

using namespace std;
using namespace essentia;
using namespace essentia::standard;

namespace AudioReader {
inline void read_audio(string path_to_audio, vector<Real>& buffer)
{
    essentia::init();
    AlgorithmFactory& factory = standard::AlgorithmFactory::instance();

    Algorithm* audio = factory.create("EasyLoader",
                                      "filename", path_to_audio,
                                      "sampleRate", 11025,
                                      "replayGain", 0);


    audio->output("audio").set(buffer);

    audio->compute();

    delete audio;
    essentia::shutdown();
}
}
#endif // AUDIOREADER_H
