#include <essentia/algorithmfactory.h>

#include "audiothumbnail.h"
#include "ui_audiothumbnail.h"
#include "audioreader.h"
#include "libs/qcustomplot/qcustomplot.h"
#include "utils.h"

#define SAMPLE_RATE 11025

using namespace std;
using namespace essentia;
using namespace essentia::standard;

AudioThumbnail::AudioThumbnail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioThumbnail)
{
    ui->setupUi(this);
    ui->waveform->addGraph();

    ui->waveform->yAxis->setVisible(true);
    ui->waveform->xAxis->setVisible(false);
//    ui->waveform->xAxis->setTicks(false);
    ui->waveform->yAxis->setTicks(false);
    ui->waveform->xAxis->setPadding(0);
    ui->waveform->yAxis->setPadding(0);
    ui->waveform->graph(0)->setAntialiased(true);
    ui->waveform->setContentsMargins(0,0,0,0);
    ui->waveform->yAxis->setRange(-1,1);
    ui->waveform->adjustSize();

    ui->playButton->setVisible(false);
    ui->filenameLabel->setVisible(false);
    ui->filepathLabel->setVisible(false);
    ui->fileIconLabel->setVisible(false);

    ui->waveform->setVisible(false);
}

AudioThumbnail::~AudioThumbnail()
{
    delete ui;
}


void AudioThumbnail::setAudio(QUrl pathToAudio)
{
    if (audioFile == pathToAudio)
       return;

    readAudio(pathToAudio);

    ui->filenameLabel->setText(pathToAudio.fileName());
    ui->filepathLabel->setText(pathToAudio.path().remove(pathToAudio.fileName()));

    ui->noFileSelectedLabel->setVisible(false);
    ui->playButton->setVisible(true);
    ui->filenameLabel->setVisible(true);
    ui->fileIconLabel->setVisible(true);
    ui->filepathLabel->setVisible(true);
    ui->waveform->setVisible(true);
    ui->filepathLabel->setStyleSheet("QLabel { color : rgb(230,230,230); }");

}

void AudioThumbnail::readAudio(QUrl pathToAudio){
//    essentia::init();
//    AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
//    Algorithm* audio = factory.create("EasyLoader", "filename", pathToAudio.toString().toStdString(), "sampleRate", SAMPLE_RATE, "replayGain", 0);

//    vector<Real> buffer;
//    audio->output("audio").set(buffer);

//    audio->compute();
//    delete audio;
//    essentia::shutdown();

//    x.resize(buffer.size());
//    y.resize(buffer.size());

//    for (int i = 0; i < buffer.size(); ++i)
//        y[i] = buffer[i];
//    iota(x.begin(), y.end(), 0);
//    ui->waveform->graph(0)->setData(x, y);
//    ui->waveform->xAxis->setRange(0, x.last());
//    ui->waveform->replot();
//}


    vector<Real> buffer;

    AudioReader::read_audio(pathToAudio.toString().toStdString(), buffer);

    x.resize(buffer.size());
    y.resize(buffer.size());

    for (int i = 0; i < buffer.size(); ++i)
        y[i] = buffer[i];


    iota(x.begin(), x.end(), 0);

    ui->waveform->graph(0)->setData(x, y);

    ui->waveform->xAxis->setRange(0, x.last());

    ui->waveform->replot();
}


void AudioThumbnail::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::black);
    painter.fillRect(this->rect(), Qt::white);
}

void AudioThumbnail::setIsPlaying(bool isPlaying) {
    if (isPlaying)
        ui->playButton->setChecked(true);
    else
        ui->playButton->setChecked(false);
}

void AudioThumbnail::playButtonToggled() {
    emit playButtonClicked();
}
