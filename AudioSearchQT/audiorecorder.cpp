/**
 * audiorecorder.h
 *
 * Implements the SoundSeek main window
 * Based on the Qt audiorecorder example, used under the BSD license
 */
/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QAudioProbe>
#include <QAudioRecorder>
#include <QDir>
#include <QFileDialog>
#include <QMediaRecorder>
#include <QVector>
#include <QTime>
#include <QFile>
#include <QDir>

#include "essentia/essentiamath.h"

#include "../src/FileUtils.h"
#include "audiorecorder.h"
#include "utils.h"
#include "audioreader.h"

#include "ui_audiorecorder.h"


using namespace std;
using namespace boost;
using namespace boost::filesystem;

AudioRecorder::AudioRecorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AudioRecorder), mediaPlayer(parent), errorMessage(parent), graph(this),
    outputLocationSet(false), queryRecorded(false), topResults(true), searchEngine()
{
    ui->setupUi(this);

    QDir tmp(QUERY_DIR);
    if (!tmp.exists())
       tmp.mkpath(".");

    ui->waveform->yAxis->setVisible(false);
    ui->waveform->xAxis->setVisible(false);
//    ui->waveform->xAxis->setTicks(false);
//    ui->waveform->yAxis->setTicks(false);
    ui->waveform->xAxis->setPadding(0);
    ui->waveform->yAxis->setPadding(0);
    ui->waveform->addGraph();
    ui->waveform->graph(0)->setAntialiased(true);
    ui->waveform->setContentsMargins(0,0,0,0);
    ui->waveform->yAxis->setRange(-1,1);
    ui->waveform->adjustSize();
//    ui->waveform->layout()->setMargin(5);

    graph.setAudioParent(this);
    audioRecorder = new QAudioRecorder(this);
    probe = new QAudioProbe;
//    connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)),
//            this, SLOT(processBuffer(QAudioBuffer)));
    probe->setSource(audioRecorder);


    //audio devices
    ui->audioDeviceBox->addItem(tr("Default"), QVariant(QString()));
    foreach (const QString &device, audioRecorder->audioInputs()) {
        ui->audioDeviceBox->addItem(device, QVariant(device));
    }


    ui->searchButton->setEnabled(false);
    ui->playQueryButton->setEnabled(false);
    ui->resultViewButton->setEnabled(false);
    ui->graphLayout->addWidget(&graph);
    ui->topResults->setEnabled(false);
    ui->ResultsLabel->setEnabled(false);

    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this,
            SLOT(updateProgress(qint64)));
    connect(audioRecorder, SIGNAL(statusChanged(QMediaRecorder::Status)), this,
            SLOT(updateStatus(QMediaRecorder::Status)));
    connect(audioRecorder, SIGNAL(stateChanged(QMediaRecorder::State)),
            this, SLOT(onStateChanged(QMediaRecorder::State)));
    connect(audioRecorder, SIGNAL(error(QMediaRecorder::Error)), this,
            SLOT(displayErrorMessage()));
}

AudioRecorder::~AudioRecorder()
{
    QDir tmp(QUERY_DIR);
    tmp.removeRecursively();
    delete audioRecorder;
    delete probe;
}

void AudioRecorder::updateProgress(qint64 duration)
{
    if (audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    //ui->statusbar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecorder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        statusMessage = tr("Stopped");
    default:
        break;
    }

    //if (audioRecorder->error() == QMediaRecorder::NoError)
        //ui->statusbar->showMessage(statusMessage);
}

void AudioRecorder::onStateChanged(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::RecordingState:
        ui->recordButton->setText(tr("Stop"));
        //ui->pauseButton->setText(tr("Pause"));
        break;
    case QMediaRecorder::PausedState:
        ui->recordButton->setText(tr("Stop"));
        //ui->pauseButton->setText(tr("Resume"));
        break;
    case QMediaRecorder::StoppedState:
        ui->recordButton->setText(tr("Record"));
        //ui->pauseButton->setText(tr("Pause"));
        break;
    }

    //ui->pauseButton->setEnabled(audioRecorder->state() != QMediaRecorder::StoppedState);
}

static QVariant boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void AudioRecorder::toggleRecord()
{
    if (audioRecorder->state() == QMediaRecorder::StoppedState) {
        audioRecorder->setAudioInput(boxValue(ui->audioDeviceBox).toString());

        queryPath = QUrl::fromLocalFile(QUERY_DIR+QString(QUERY_PREFIX)+QTime::currentTime().toString(Qt::DefaultLocaleLongDate)+".wav");
        setOutputLocation(queryPath);
        QAudioEncoderSettings settings;
//        settings.setCodec(boxValue(ui->audioCodecBox).toString());
//        settings.setSampleRate(boxValue(ui->sampleRateBox).toInt());
//        settings.setBitRate(boxValue(ui->bitrateBox).toInt());
//        settings.setChannelCount(boxValue(ui->channelsBox).toInt());
//        settings.setQuality(QMultimedia::EncodingQuality(ui->qualitySlider->value()));
//        settings.setEncodingMode(ui->constantQualityRadioButton->isChecked() ?
//                                 QMultimedia::ConstantQualityEncoding :
//                                 QMultimedia::ConstantBitRateEncoding);

//        QString container = boxValue(ui->containerBox).toString();

        settings.setSampleRate(22050);
        settings.setBitRate(64000);
        settings.setChannelCount(1);

        QString container = audioRecorder->supportedContainers()[0];

        audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
        audioRecorder->record();
    }
    else {
        audioRecorder->stop();
        queryRecorded = true;
        ui->searchButton->setEnabled(true);
        ui->playQueryButton->setEnabled(true);
        graph.clear();
        graph.addNode(nullptr, queryPath, 0);
        searchByPath(queryPath);
    }
}

void AudioRecorder::togglePause()
{
    if (audioRecorder->state() != QMediaRecorder::PausedState)
        audioRecorder->pause();
    else
        audioRecorder->record();
}

// for finding neighbors of the initial query
void AudioRecorder::search()
{
    if (!queryRecorded)
        return;
    //ui->statusbar->showMessage("Searching");

    topResults = true;
    //updateResults();

    //ui->resultViewButton->setEnabled(true);
    //ui->statusbar->showMessage("Search %d files", searchEngine.getNumSearchFiles());
    auto* parent = graph.findNode(queryPath);
    foreach(path p, searchEngine.getNearestWeighted(QUrlToPath(queryPath), 7))
    {
        graph.addNode (parent, pathToUrl(p));
    }
}

void AudioRecorder::searchByPath(QUrl searchPath)
{
    //ui->statusbar->showMessage("Searching");
    auto* parent = graph.findNode(searchPath);
    if (!parent)
        return;
    foreach(path p, searchEngine.getNearestWeighted(QUrlToPath(searchPath), 7))
    {
        cout << p << endl;
        graph.addNode (parent, pathToUrl(p.string()));
    }
}



void AudioRecorder::setSearchDirectory()
{
    QUrl dirName = QFileDialog::getExistingDirectoryUrl(0, "Open a Folder of Audio", QUrl::fromLocalFile("/Users/michael/InteractiveAudioLab/audiosearch/Audio/"), QFileDialog::ShowDirsOnly);
    searchEngine.addDirectory(QUrl::fromPercentEncoding(dirName.toLocalFile().toLocal8Bit()).toStdString(), true);
    //sleep(.01);

    searchEngine.calcAllDistances();
    ui->searchDirLabel->setText(QUrl::fromPercentEncoding(dirName.toLocalFile().toLocal8Bit()));
}

void AudioRecorder::resultClicked(){
    //qDebug ("result clicked");
    auto currentItem = ui->topResults->currentItem();
    auto audio = QUrl::fromLocalFile(currentItem->text());
    playAudio(audio);
}

void AudioRecorder::playQuery()
{
    playAudio(queryPath);
}

void AudioRecorder::toggleResultView()
{
    if (topResults){
        ui->resultViewButton->setText("Show Nearest by Feature");
    }
    else
    {
        ui->resultViewButton->setText("Show Top Results");
    }
    updateResults();
    topResults = !topResults;
}

void AudioRecorder::updateResults()
{
//    ui->topResults->clear();
//    if (topResults)
//    {
//        foreach(searchResult r, searchEngine.getTopResults(20))
//        {
//            ui->topResults->addItem(QString::fromUtf8(r.p.c_str()));
//        }
//    }
//    else
//    {
//        foreach(searchResult r, searchEngine.getNearestResultByFeature())
//        {
//            ui->topResults->addItem(QString::fromUtf8(r.p.c_str()));
//        }
//    }
}

void AudioRecorder::setOutputLocation(QUrl path)
{
    //QString fileName = QFileDialog::getSaveFileName();
    //audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    audioRecorder->setOutputLocation(path);

    outputLocationSet = true;
}

void AudioRecorder::displayErrorMessage()
{
    //ui->statusbar->showMessage(audioRecorder->errorString());
}

void AudioRecorder::playAudio(QUrl pathToAudio)
{
    if (mediaPlayer.media().canonicalUrl() != pathToAudio) {
        vector<Real> buffer;
        AudioReader::read_audio(pathToAudio.toString().toStdString(), buffer);
        wav_x.resize(buffer.size());
        wav_y.resize(buffer.size());

        for (int i = 0; i < buffer.size(); ++i)
            wav_y[i] = buffer[i];
        iota(wav_x.begin(), wav_x.end(), 0);
        ui->waveform->graph(0)->setData(wav_x, wav_y);
        ui->waveform->xAxis->setRange(0, wav_x.last());
        ui->waveform->replot();
    }
    if (mediaPlayer.state() == QMediaPlayer::PlayingState || mediaPlayer.media().canonicalUrl() == pathToAudio)
        mediaPlayer.stop();

    mediaPlayer.setMedia(pathToAudio);
    mediaPlayer.setVolume(100);

    mediaPlayer.play();

    ui->waveformLabel->setText(mediaPlayer.media().canonicalUrl().fileName());
}

void AudioRecorder::playLastAudio()
{
    mediaPlayer.setVolume(100);

    mediaPlayer.play();
}

void AudioRecorder::stopAudio()
{
    mediaPlayer.stop();
    //ui->statusbar->clearMessage();
}

void AudioRecorder::toggleAudio()
{
    if (mediaPlayer.state() == QMediaPlayer::PlayingState)
        mediaPlayer.stop();
    else
        mediaPlayer.play();
}

void AudioRecorder::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
        toggleAudio();
        break;
    default:
        break;
    }
}
