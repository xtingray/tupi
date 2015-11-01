INCLUDEPATH += $$LIBRARY_DIR

unix {
    LIBS += -L$$LIBRARY_DIR -ltupilibrary
}

win32 {
    LIBS += -L$$LIBRARY_DIR/release/ -ltupilibrary
}

linux-g++ {
    PRE_TARGETDEPS += $$LIBRARY_DIR/libtupilibrary.so
}
