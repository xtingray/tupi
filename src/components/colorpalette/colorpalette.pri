INCLUDEPATH += $$COLORPALETTE_DIR
LIBS += -L$$COLORPALETTE_DIR -lcolorpalette

linux-g++ {
    PRE_TARGETDEPS += $$COLORPALETTE_DIR/libcolorpalette.so
}

