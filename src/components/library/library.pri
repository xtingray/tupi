INCLUDEPATH += $$LIBRARY_DIR
LIBS += -L$$LIBRARY_DIR -ltupilibrary

linux-g++ {
    PRE_TARGETDEPS += $$LIBRARY_DIR/libtupilibrary.so
}
