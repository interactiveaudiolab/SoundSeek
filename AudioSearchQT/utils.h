#ifndef UTILS_H
#define UTILS_H

#include <QUrl>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

inline string QUrlToString(QUrl url)
{
    return QUrl::fromPercentEncoding(url.toLocalFile().toLocal8Bit()).toStdString();
}

inline path QUrlToPath(QUrl url)
{
    return path(QUrl::fromPercentEncoding(url.toLocalFile().toLocal8Bit()).toStdString());
}

inline QUrl pathToUrl(path p)
{
    return QUrl::fromLocalFile(QString::fromUtf8(p.c_str()));
}

#endif // UTILS_H
