#!/bin/bash 
export $EXEC_PATH = /Applications/AudioSearchQT.app
$HOME/Qt5.8/5.8/clang_64/bin/macdeployqt $EXEC_PATH

cp -R /Users/michael/Qt5.8/5.8/clang_64/lib/QtCore.framework/ $EXEC_PATH/Contents/Frameworks/

cp -R /Users/michael/Qt5.8/5.8/clang_64/lib/QtMultimedia.framework/ $EXEC_PATH/Contents/Frameworks/

cp -R /Users/michael/Qt5.8/5.8/clang_64/lib/QtCore.framework/ $EXEC_PATH/Contents/Frameworks/

