INCLUDEPATH += $$BRUSH_DIR

unix {
    LIBS += -L$$BRUSH_DIR -ltupibrush
}

win32 {
    LIBS += -L$$BRUSH_DIR/release/ -ltupibrush
}

linux-g++ {
    PRE_TARGETDEPS += $$BRUSH_DIR/libtupibrush.so
}
