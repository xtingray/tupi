INCLUDEPATH += $$COLORPALETTE_DIR

unix {
    LIBS += -L$$COLORPALETTE_DIR -ltupicolorpalette
}

win32 {
    LIBS += -L$$COLORPALETTE_DIR/release -ltupicolorpalette
}

linux-g++ {
    PRE_TARGETDEPS += $$COLORPALETTE_DIR/libtupicolorpalette.so
}

