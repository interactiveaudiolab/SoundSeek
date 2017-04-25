#ifndef AUDIOTHUMBNAIL_H
#define AUDIOTHUMBNAIL_H

#include <QWidget>
#include <QVector>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui {
class AudioThumbnail;
}
QT_END_NAMESPACE

class AudioThumbnail : public QWidget
{
    Q_OBJECT

public:
    explicit AudioThumbnail(QWidget *parent = 0);
    ~AudioThumbnail();

    void setAudio(QUrl pathToAudio);
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

public slots:
    void setIsPlaying(bool isPlaying);

private slots:
    void playButtonToggled();

signals:
    void playButtonClicked();

private:
    Ui::AudioThumbnail *ui;

    QVector<double> x, y;
    QUrl audioFile;

    void readAudio(QUrl pathToAudio);
};

#endif // AUDIOTHUMBNAIL_H
