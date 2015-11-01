INCLUDEPATH += $$DEBUG_DIR

unix {
    LIBS += -L$$DEBUG_DIR -ltupidebug
}

win32 {
    LIBS += -L$$DEBUG_DIR/release/ -ltupidebug
}

linux-g++ {
    PRE_TARGETDEPS += $$DEBUG_DIR/libtupidebug.so
}
