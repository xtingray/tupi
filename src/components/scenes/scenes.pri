INCLUDEPATH += $$SCENES_DIR
LIBS += -L$$SCENES_DIR -lscenes

linux-g++ {
    PRE_TARGETDEPS += $$SCENES_DIR/libscenes.so
}
