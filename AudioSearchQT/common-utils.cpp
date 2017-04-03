/*
Created by github user andrey-str
https://gist.github.com/andrey-str/1795f40ee73e6e57b9a7
*/

#include <QProcess>

void revealFile(QWidget* parent, const QString &pathToReveal) {

    // See http://stackoverflow.com/questions/3490336/how-to-reveal-in-finder-or-show-in-explorer-with-qt
    // for details

    // Mac, Windows support folder or file.
#if defined(Q_OS_WIN)
    const QString explorer = Environment::systemEnvironment().searchInPath(QLatin1String("explorer.exe"));
    if (explorer.isEmpty()) {
        QMessageBox::warning(parent,
                             tr("Launching Windows Explorer failed"),
                             tr("Could not find explorer.exe in path to launch Windows Explorer."));
        return;
    }
    QString param;
    if (!QFileInfo(pathIn).isDir())
        param = QLatin1String("/select,");
    param += QDir::toNativeSeparators(pathIn);
    QString command = explorer + " " + param;
    QString command = explorer + " " + param;
    QProcess::startDetached(command);

#elif defined(Q_OS_MAC)
    Q_UNUSED(parent)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
            << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
            .arg(pathToReveal);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
            << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
    // we cannot select a file here, because no file browser really supports it...
    const QFileInfo fileInfo(pathIn);
    const QString folder = fileInfo.absoluteFilePath();
    const QString app = Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings());
    QProcess browserProc;
    const QString browserArgs = Utils::UnixUtils::substituteFileBrowserParameters(app, folder);
    if (debug)
        qDebug() <<  browserArgs;
    bool success = browserProc.startDetached(browserArgs);
    const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
    success = success && error.isEmpty();
    if (!success)
        showGraphicalShellError(parent, app, error);
#endif

}
