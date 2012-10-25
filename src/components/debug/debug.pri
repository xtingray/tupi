INCLUDEPATH += $$DEBUG_DIR
LIBS += -L$$DEBUG_DIR -ltupidebug

linux-g++ {
    PRE_TARGETDEPS += $$DEBUG_DIR/libtupidebug.so
}
