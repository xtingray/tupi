INCLUDEPATH += $$PEN_DIR

unix {
    LIBS += -L$$PEN_DIR -ltupipen
}

win32 {
    LIBS += -L$$PEN_DIR/release/ -ltupipen
}

linux-g++ {
    PRE_TARGETDEPS += $$PEN_DIR/libtupipen.so
}
