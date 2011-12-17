INCLUDEPATH += $$PAINTAREA_DIR
LIBS += -L$$PAINTAREA_DIR -lpaintarea

linux-g++ {
PRE_TARGETDEPS += $$PAINTAREA_DIR/libpaintarea.a
}

