INCLUDEPATH += $$SCENES_DIR

unix {
    LIBS += -L$$SCENES_DIR -ltupiscenes
}

win32 {
    LIBS += -L$$SCENES_DIR/release/ -ltupiscenes
}

linux-g++ {
    PRE_TARGETDEPS += $$SCENES_DIR/libtupiscenes.so
}
