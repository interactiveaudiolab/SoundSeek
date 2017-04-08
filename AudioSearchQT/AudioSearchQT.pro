TEMPLATE = app
TARGET = AudioSearchQT

QT += multimedia

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
    starpolygon.h

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
    ../src/tests.cpp

FORMS += audiorecorder.ui

target.path = /Applications/AudioSearchQT
INSTALLS += target

QT+=widgets

QMAKE_LFLAGS += -F /system/Library/Frameworks/ApplicationServices.framework/Frameworks/ImageIo.framework
LIBS += -framework CoreFoundation

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/ -lboost_system-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/ -lboost_system-mt
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib -lboost_system-mt

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/include


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/libboost_system.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/libboost_system.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/release/boost_systemt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/debug/boost_system.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/Cellar/boost/1.62.0/lib/libboost_system.a


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


INCLUDEPATH += $$PWD/../../../../../usr/local/include/eigen3
DEPENDPATH += $$PWD/../../../../../usr/local/include/eigen3

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/lib/release/ -lessentia
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/lib/debug/ -lessentia
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/lib/ -lessentia

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/essentia/2.1_beta3/include

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
