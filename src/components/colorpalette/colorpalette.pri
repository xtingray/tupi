INCLUDEPATH += $$COLORPALETTE_DIR
LIBS += -L$$COLORPALETTE_DIR -ltupicolorpalette

linux-g++ {
    PRE_TARGETDEPS += $$COLORPALETTE_DIR/libtupicolorpalette.so
}

