#ifndef AUDIOTHUMBNAIL_H
#define AUDIOTHUMBNAIL_H
#include <QtWidgets>
#include <QVector>

#include <essentia/algorithmfactory.h>
#include "libs/qcustomplot/qcustomplot.h"
#include "utils.h"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

class AudioThumbnail
{
public:
    AudioThumbnail() {
        waveform.addGraph();
    }

    void setAudio(QUrl pathToAudio) {
        if (audioFile != pathToAudio)
            read_audio(QUrlToString(pathToAudio));
    }

private:
    QCustomPlot waveform;
    QLabel filenameLabel, durationLabel, sampleRateLabel, bitRateLabel, channelsLabel;
    QVector<double> x, y;
    QUrl audioFile;

    void read_audio(string pathToAudio)
    {
        essentia::init();
        AlgorithmFactory& factory = standard::AlgorithmFactory::instance();

        Algorithm* audio = factory.create("EasyLoader",
                                          "filename", pathToAudio,
                                          "sampleRate", 11025,
                                          "replayGain", 0);
        Algorithm* meta = factory.create("MetadataReader",
                                         "filename", pathToAudio);

        vector<Real> buffer;
        string duration, sampleRate, bitrate, channels;
        audio->output("audio").set(buffer);

        meta->output("duration").set(duration);
        meta->output("sampleRate").set(sampleRate);
        meta->output("bitrate").set(bitrate);
        meta->output("channels").set(channels);

        audio->compute();
        meta->compute();

        delete meta;
        delete audio;
        essentia::shutdown();

        durationLabel.setText(QString::fromStdString(duration));
        sampleRateLabel.setText(QString::fromStdString(sampleRate));
        bitRateLabel.setText(QString::fromStdString(bitrate));
        channelsLabel.setText(QString::fromStdString(channels));

        x.resize(buffer.size());
        y.resize(buffer.size());

        for (int i = 0; i < buffer.size(); ++i)
            y[i] = buffer[i];
        iota(x.begin(), y.end(), 0);
        waveform.graph(0)->setData(x, y);
        waveform.xAxis->setRange(0, x.last());
        waveform.replot();

    }
};

#endif // AUDIOTHUMBNAIL_H
