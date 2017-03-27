//
//  FeatureExtractor.cpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 12/13/16.
//

#include "FeatureExtractor.h"

FeatureExtractor::FeatureExtractor (){};

FeatureExtractor::~FeatureExtractor (){};

void FeatureExtractor::init (int frame_size, int hop_size, int sample_rate)
{
    this->frame_size = frame_size;
    this->hop_size = hop_size;
    this->resample_rate = sample_rate;
}

void FeatureExtractor::extract (path file_path, bool write_analysis_file) const
{
    DBG ("\nAnalyzing " << file_path.string ());
    Pool pool;

    string file_name = file_path.string ();
    pool.add ("path", file_name, true);

    // Based off of tutorial from http://essentia.upf.edu/documentation/howto_standard_extractor.html
    essentia::init ();

    AlgorithmFactory &factory = standard::AlgorithmFactory::instance ();

    /*
     Load Audio
     */
    Algorithm *audio = factory.create ("EasyLoader", "filename", file_name, "sampleRate", resample_rate);

    vector<Real> audio_buffer;

    //    Real sample_rate;
    //    int bit_rate;
    //    string codec, md5;
    //    int num_channels;

    audio->output ("audio").set (audio_buffer);
    //    audio->output ("sampleRate").set (sample_rate);
    //    audio->output ("bit_rate").set (bit_rate);
    //    audio->output ("codec").set (codec);
    //    audio->output ("numberChannels").set (num_channels);
    //    audio->output ("md5").set (md5);

    audio->compute ();

    //    pool.add ("sample_rate", sample_rate);
    //    pool.add ("bit_rate", bit_rate);
    //    pool.add ("codec", codec);
    //    pool.add ("num_channels", num_channels);

    /*
     Create feature algorithms
     */

    Algorithm *fc = factory.create ("FrameCutter", "frameSize", frame_size, "hopSize", hop_size, "startFromZero", true);

    Algorithm *find_silence = factory.create ("StartStopSilence", "threshold", -36);

    Algorithm *w = factory.create ("Windowing", "type", "hann");

    Algorithm *spec = factory.create ("Spectrum");
    Algorithm *mfcc = factory.create ("MFCC", "numberCoefficients", NUM_MFCC);

    Algorithm *pitch = factory.create ("PitchYinFFT", "frameSize", frame_size, "sampleRate", resample_rate);

    Algorithm *centroid = factory.create ("Centroid", "range", resample_rate / 2.f);

    // Spectral kurtosis, spread
    Algorithm *central_moments = factory.create ("CentralMoments");
    Algorithm *distribution_shape = factory.create ("DistributionShape");

    Algorithm *loudness = factory.create ("Loudness");

    /*
     Routing
     */
    // FrameCutter
    fc->input ("signal").set (audio_buffer);

    // FrameCutter -> Remove Silence -> Windowing -> Spectrum
    vector<Real> frame, windowed_frame;
    Real sound_start, sound_end;

    fc->output ("frame").set (frame);

    /*
    find_silence->input("frame").set(frame);
    find_silence->output("startFrame").set(sound_start);
    find_silence->output("stopFrame").set(sound_end);
    */

    w->input ("frame").set (frame);

    w->output ("frame").set (windowed_frame);
    spec->input ("frame").set (windowed_frame);

    // Audio -> loudness,
    Real loudness_val;
    loudness->input ("signal").set (windowed_frame);
    loudness->output ("loudness").set (loudness_val);

    // Spectrum -> MFCC, pitch, centroid, kurtosis, spread
    vector<Real> spectrum, mfcc_coeffs, mfcc_bands, cent_moments;
    Real pitch_Hz, pitch_confidence, spec_centroid, spec_kurtosis, spec_spread, spec_skew;

    spec->output ("spectrum").set (spectrum);

    mfcc->input ("spectrum").set (spectrum);
    pitch->input ("spectrum").set (spectrum);
    centroid->input ("array").set (spectrum);
    central_moments->input ("array").set (spectrum);
    distribution_shape->input ("centralMoments").set (cent_moments);

    mfcc->output ("bands").set (mfcc_bands);
    mfcc->output ("mfcc").set (mfcc_coeffs);
    pitch->output ("pitch").set (pitch_Hz);
    pitch->output ("pitchConfidence").set (pitch_confidence);
    centroid->output ("centroid").set (spec_centroid);
    central_moments->output ("centralMoments").set (cent_moments);
    distribution_shape->output ("kurtosis").set (spec_kurtosis);
    distribution_shape->output ("spread").set (spec_spread);
    distribution_shape->output ("skewness").set (spec_skew);

    /*
     Compute features
     */

    // frame-based features
    while (true)
    {
        fc->compute ();

        if (!frame.size ()) break;

        if (isSilent (frame)) continue;

        w->compute ();
        spec->compute ();
        mfcc->compute ();
        pitch->compute ();
        centroid->compute ();
        central_moments->compute ();
        distribution_shape->compute ();
        loudness->compute ();

        for (int i = 0; i < NUM_MFCC; ++i) pool.add ("mfcc_" + to_string (i), mfcc_coeffs[i], true);

        pool.add ("pitch", pitch_Hz, true);
        pool.add ("pitch_confidence", pitch_confidence, true);
        pool.add ("spectral_centroid", spec_centroid, true);
        pool.add ("spectral_kurtosis", spec_kurtosis, true);
        pool.add ("spectral_spread", spec_spread, true);
        pool.add ("loudness", loudness_val, true);
    }

    /*
     Output
     */
    string write_path = "-";
    if (write_analysis_file) write_path = FileUtils::getAnalysisFilePath (file_path).string ();

    Algorithm *output = factory.create ("YamlOutput", "doubleCheck", true, "filename", write_path, "format", "json",
                                        "writeVersion", false);
    output->input ("pool").set (pool);
    output->compute ();

    /*
     Cleanup
     */
    delete audio;
    delete fc;
    delete find_silence;
    delete w;
    delete spec;
    delete mfcc;
    delete pitch;
    delete centroid;
    delete central_moments;
    delete distribution_shape;
    delete loudness;
    delete output;

    essentia::shutdown ();
}
