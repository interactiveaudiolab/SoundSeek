TEMPLATE = app
TARGET = AudioSearchQT

QT += multimedia core

CONFIG += static
QMAKE_RPATHDIR += @executable_path/../Frameworks
QMAKE_RPATHDIR += @executable_path/lib

win32:INCLUDEPATH += $$PWD

HEADERS = \
    audiorecorder.h \
    ../src/libs/json.hpp \
    ../src/AudioFeatures.h \
    ../src/AudioObject.h \
    ../src/AudioSearchEngine.h \
    ../src/catch.hpp \
    ../src/Common.h \
    ../src/Distance.h \
    ../src/FeatureExtractor.h \
    ../src/FileUtils.h \
    ../src/ThreadPool.h \
    edge.h \
    graphwidget.h \
    node.h \
    ../src/libs/ThreadPool.h \
    starpolygon.h \
    utils.h \
    ../src/libs/MULTI_ARRAY_SERIALIZATION.hpp \
    libs/qcustomplot/qcustomplot.h \
    audioreader.h \
    audiothumbnail.h

SOURCES = \
    main.cpp \
    audiorecorder.cpp \
    ../src/AudioSearchEngine.cpp \
    ../src/FeatureExtractor.cpp \
    node.cpp \
    graphwidget.cpp \
    edge.cpp \
    ../src/Distance.cpp \
    common-utils.cpp \
    ../src/FileUtils.cpp \
    libs/qcustomplot/qcustomplot.cpp \
    audiothumbnail.cpp

FORMS += audiorecorder.ui \
    audiothumbnail.ui

target.path = /Applications/
INSTALLS += target

QT+=widgets printsupport

QMAKE_LFLAGS += -F /system/Library/Frameworks/ApplicationServices.framework/Frameworks/ImageIo.framework
LIBS += -framework CoreFoundation

# aquila

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lAquila
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lAquila
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib -lAquila

INCLUDEPATH += $$PWD/../../../../../usr/local/include/aquila
DEPENDPATH += $$PWD/../../../../../usr/local/include/aquila

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/release/libAquila.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/debug/libAquila.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/release/Aquila.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/debug/Aquila.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/libAquila.a


# eigen
INCLUDEPATH += $$PWD/../../../../../usr/local/include/eigen3
DEPENDPATH += $$PWD/../../../../../usr/local/include/eigen3




# boost
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/ -lboost_system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/ -lboost_system
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/ -lboost_system

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/libboost_system.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/libboost_system.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/boost_system.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/boost_system.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/libboost_system.a




# boost::filesystem
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/ -lboost_filesystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/ -lboost_filesystem
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/ -lboost_filesystem

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/libboost_filesystem.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/libboost_filesystem.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/boost_filesystem.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/boost_filesystem.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/libboost_filesystem.a





# boost::serialization
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/ -lboost_serialization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/ -lboost_serialization
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/ -lboost_serialization

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/libboost_serialization.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/libboost_serialization.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/boost_serialization.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/boost_serialization.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/libboost_serialization.a





# essentia
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/lib/release/ -lessentia
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/lib/debug/ -lessentia
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/lib/ -lessentia

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/include

RESOURCES += \
    images.qrc

DISTFILES +=
