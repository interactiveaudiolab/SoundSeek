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

#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QMainWindow>
#include <QMediaRecorder>
#include <QUrl>
#include <QErrorMessage>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QKeyEvent>

#include "boost/filesystem.hpp"
#include "../src/AudioSearchEngine.h"
#include "graphwidget.h"
#include "node.h"

#define RECORD_PATH "/tmp/audiosearch_query.wav"


QT_BEGIN_NAMESPACE
namespace Ui { class AudioRecorder; }
class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;
class QErrorMessage;
QT_END_NAMESPACE

using namespace boost;

class AudioRecorder : public QMainWindow
{
    Q_OBJECT

public:
    AudioRecorder(QWidget *parent = 0);
    ~AudioRecorder();

    void playAudio(QUrl pathToAudio);
    void searchByPath(QUrl searchPath = QUrl());
    void toggleAudio();

private slots:
    void setOutputLocation(QUrl path);
    void togglePause();
    void toggleRecord();
    void setSearchDirectory();
    void search();
    void resultClicked();
    void playQuery();
    void toggleResultView();
    void stopAudio();
    void playLastAudio();

    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    QUrl pathToUrl(path p);
    std::string QUrlToString(QUrl path);
    void updateResults();

    Ui::AudioRecorder *ui;

    QAudioRecorder *audioRecorder;
    QAudioProbe *probe;
    QErrorMessage errorMessage;

    QMediaPlayer mediaPlayer;

    GraphWidget graph;

    boost::filesystem::path queryPath;
    AudioSearchEngine searchEngine;
    bool outputLocationSet;
    bool queryRecorded;
    bool topResults;
};

#endif // AUDIORECORDER_H
