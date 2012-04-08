INCLUDEPATH += $$DEBUG_DIR
LIBS += -L$$DEBUG_DIR -ldebug

linux-g++ {
    PRE_TARGETDEPS += $$DEBUG_DIR/libdebug.so
}
