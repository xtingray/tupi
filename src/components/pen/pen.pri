INCLUDEPATH += $$PEN_DIR
LIBS += -L$$PEN_DIR -lpen

linux-g++ {
    PRE_TARGETDEPS += $$PEN_DIR/libpen.so
}
