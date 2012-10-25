INCLUDEPATH += $$KINAS_DIR
LIBS += -L$$KINAS_DIR -ltupikinas

linux-g++ {
    PRE_TARGETDEPS += $$KINAS_DIR/libtupikinas.so
}

