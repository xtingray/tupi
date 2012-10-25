INCLUDEPATH += $$PEN_DIR
LIBS += -L$$PEN_DIR -ltupipen

linux-g++ {
    PRE_TARGETDEPS += $$PEN_DIR/libtupipen.so
}
