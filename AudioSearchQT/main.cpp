#include "audiorecorder.h"

#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AudioRecorder mainWindow;
    mainWindow.show();

    return app.exec();
}
