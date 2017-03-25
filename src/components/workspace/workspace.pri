INCLUDEPATH += /usr/include/qt5/QtMultimedia /usr/include/qt5/QtMultimediaWidgets
INCLUDEPATH += $$WORKSPACE_DIR 

unix {
    LIBS += -L$$WORKSPACE_DIR -ltupiworkspace
}

win32 {
    LIBS += -L$$WORKSPACE_DIR/release/ -ltupiworkspace
}

linux-g++ {
PRE_TARGETDEPS += $$WORKSPACE_DIR/libtupiworkspace.so
}

