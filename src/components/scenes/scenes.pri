INCLUDEPATH += $$SCENES_DIR
LIBS += -L$$SCENES_DIR -ltupiscenes

linux-g++ {
    PRE_TARGETDEPS += $$SCENES_DIR/libtupiscenes.so
}
