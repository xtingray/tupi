INCLUDEPATH += /usr/include/qt5/QtMultimedia /usr/include/qt5/QtMultimediaWidgets
INCLUDEPATH += $$PAINTAREA_DIR 

unix {
    LIBS += -L$$PAINTAREA_DIR -ltupipaintarea
}

win32 {
    LIBS += -L$$PAINTAREA_DIR/release/ -ltupipaintarea
}

linux-g++ {
PRE_TARGETDEPS += $$PAINTAREA_DIR/libtupipaintarea.so
}

