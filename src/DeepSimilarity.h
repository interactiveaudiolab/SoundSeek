//
//  DeepSimilarity.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 5/25/17.
//  Copyright © 2017 Michael Donovan. All rights reserved.
//

#ifndef DeepSimilarity_h
#define DeepSimilarity_h

#include "boost/filesystem.hpp"
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <essentia/algorithmfactory.h>
#include <iostream>
#include <memory>

#include <caffe/blob.hpp>
#include <caffe/caffe.hpp>
#include <caffe/net.hpp>
#include <caffe/util/io.hpp>

//#include
//"/usr/local/Cellar/python/2.7.13/Frameworks/Python.framework/Versions/2.7/include/python2.7/Python.h"
#define PROTOTXT                                                                                   \
    "/Users/michael/InteractiveAudioLab/NN_models/caffeVersion/keras2caffe_full.prototxt"
#define CAFFEMODEL                                                                                 \
    "/Users/michael/InteractiveAudioLab/NN_models/caffeVersion/keras2caffe_final.caffemodel"
#define SCRIPT_PATH "/Users/michael/InteractiveAudioLab/siamese_demo_code/keras2caffe_demo.py"
#define PYTHONPATH "/usr/local/bin/python"

#define SR 16000
#define LENGTH_IN_SEC 3

using boost::filesystem::path;
using namespace std;
using namespace caffe;
using namespace essentia;
using namespace essentia::standard;

namespace Distance {
class CaffeNetSimilarity
{
  public:
    CaffeNetSimilarity(path prototxt, path caffemodel, bool gpu = false)
    {
        if (!is_regular_file(prototxt))
            throw invalid_argument("File does not exist: " + prototxt.string());
        if (!is_regular_file(caffemodel))
            throw invalid_argument("File does not exist: " + prototxt.string());

        if (gpu) {
            Caffe::set_mode(Caffe::GPU);
            Caffe::SetDevice(0);
        } else {
            Caffe::set_mode(Caffe::CPU);
        }

        net.reset(new Net<float>(prototxt.string(), TEST));
        net->CopyTrainedLayersFrom(caffemodel.string());
    }
    ~CaffeNetSimilarity() {}

    float similarity(path a, path b)
    {
        auto mgram_a = melspectrogram(a);
        auto mgram_b = melspectrogram(b);

        int width = LENGTH_IN_SEC * SR;
        int height = mgram_a[0].size();

        //        BlobProto blob_proto_a;
        //        blob_proto_a.set_num(1);
        //        blob_proto_a.set_channels(1);
        //        blob_proto_a.set_height(height);
        //        blob_proto_a.set_width(width);
        //
        //        for (int i = 0; i < width; ++i) {
        //            for (int j = 0; j < height; ++j) {
        //                blob_proto_a.set_data(width * j + i, mgram_a[i][j]);
        //            }
        //        }
        //
        //        BlobProto blob_proto_b;
        //        blob_proto_b.set_num(1);
        //        blob_proto_b.set_channels(1);
        //        blob_proto_b.set_height(height);
        //        blob_proto_b.set_width(width);
        //
        //        for (int i = 0; i < width; ++i) {
        //            for (int j = 0; j < height; ++j) {
        //                blob_proto_b.set_data(width * j + i, mgram_b[i][j]);
        //            }
        //        }

        //        net->blob_by_name("data")->FromProto(blob_proto_a);
        //        net->blob_by_name("data_p")->FromProto(blob_proto_b);

        net->blob_by_name("data")->set_cpu_data((float*)mgram_a.data());
        net->blob_by_name("data_p")->set_cpu_data((float*)mgram_b.data());

        return net->Forward()[0]->data_at(0, 0, 0, 0);
    }

    vector<vector<float>> melspectrogram(path file_path, float sr = 16000, int frame_size = 512,
                                         int hop_size = 256, bool log = true)
    {
        // Based off of tutorial from
        // http://essentia.upf.edu/documentation/howto_standard_extractor.html
        essentia::init();

        AlgorithmFactory& factory = standard::AlgorithmFactory::instance();

        /*
         Load Audio
         */
        Algorithm* audio =
          factory.create("EasyLoader", "filename", file_path.string(), "sampleRate", sr);

        vector<Real> audio_buffer;
        audio->output("audio").set(audio_buffer);
        audio->compute();

        /*
         Create feature algorithms
         */

        Algorithm* fc = factory.create("FrameCutter", "frameSize", frame_size, "hopSize", hop_size,
                                       "startFromZero", true);

        Algorithm* w = factory.create("Windowing", "type", "hann");

        Algorithm* spec = factory.create("Spectrum");
        Algorithm* melbands =
          factory.create("MelBands", "numberBands", .5 * frame_size, /*"log", log,*/
                         "sampleRate", sr, "highFrequencyBound", sr / 2. /*,
                         "type", "magnitude"*/);

        /*
         Routing
         */
        // FrameCutter
        fc->input("signal").set(audio_buffer);

        // FrameCutter -> Windowing -> Spectrum
        vector<Real> frame, windowed_frame;

        fc->output("frame").set(frame);

        w->input("frame").set(frame);

        w->output("frame").set(windowed_frame);
        spec->input("frame").set(windowed_frame);

        // Spectrum -> melbands
        vector<Real> spectrum, mel_spectrum;

        spec->output("spectrum").set(spectrum);

        melbands->input("spectrum").set(spectrum);
        melbands->output("bands").set(mel_spectrum);

        /*
         Compute features
         */

        // frame-based features
        vector<vector<float>> mel_spectrogram;
        while (true) {
            fc->compute();

            if (!frame.size())
                break;

            if (isSilent(frame))
                continue;

            w->compute();
            spec->compute();
            melbands->compute();
            mel_spectrogram.push_back(mel_spectrum);
        }

        /*
         Cleanup
         */
        delete audio;
        delete fc;
        delete w;
        delete spec;
        delete melbands;

        essentia::shutdown();

        return mel_spectrogram;
    }

  private:
    std::shared_ptr<Net<float>> net;
};

// from
// https://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
static std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}

static double deepSimilarity(path a, path b)
{
    string command =
      string(PYTHONPATH) + " " + string(SCRIPT_PATH) + " " + a.string() + string(" ") + b.string();
    boost::replace_all(command, " ", "\ ");
    auto output = exec(command.c_str());
    DBG(output);
    return stod(output);

    //    Py_InitializeEx (0);
    //    PyRun_SimpleString ("import sys, os");
    //    //    PyRun_SimpleString ("print sys.path");
    //    string pathAppend = string ("sys.path.append('") + string (SCRIPT_PATH) + string ("')");
    //    PyRun_SimpleString (pathAppend.c_str ());
    //    pathAppend = string ("sys.path.append('") +
    //    "/Users/michael/InteractiveAudioLab/caffe/python" + string ("')");
    //    PyRun_SimpleString (pathAppend.c_str ());
    //
    //    PyObject* moduleString = PyString_FromString ((char*) "keras2caffe_demo");
    //    PyObject* module = PyImport_Import (moduleString);
    //    if (module == nullptr)
    //    {
    //        PyErr_Print ();
    //    }
    //
    //    //    Py_DecRef (moduleString);
    //
    //    PyObject* function = PyObject_GetAttrString (module, (char*) "siamese_similarity");
    //
    //    PyObject* args = PyTuple_New (2);
    //    PyTuple_SET_ITEM (args, 0, PyString_FromString (a.string ().c_str ()));
    //    PyTuple_SET_ITEM (args, 1, PyString_FromString (b.string ().c_str ()));
    //
    //    PyObject* output = PyObject_CallObject (function, args);
    //    if (output == nullptr)
    //    {
    //        PyErr_Print ();
    //        throw runtime_error ("");
    //    }
    //
    //    double result;
    //    if (PyFloat_Check (output))
    //    {
    //        result = PyFloat_AsDouble (output);
    //    }
    //    else
    //    {
    //        DBG ("function did not return float");
    //        throw runtime_error ("");
    //    }
    //    //    Py_DecRef (function);
    //    //    Py_DecRef (args);
    //
    //    Py_Finalize ();
    //    return result;
}
}

#endif /* DeepSimilarity_h */
