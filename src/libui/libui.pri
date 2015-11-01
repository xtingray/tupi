ANIMATION_DIR = ../components/animation
INCLUDEPATH += $$LIBUI_DIR
INCLUDEPATH += $$ANIMATION_DIR

unix {
    LIBS += -L$$LIBUI_DIR -ltupigui
}

win32 {
    LIBS += -L$$LIBUI_DIR/release/ -ltupigui
}

linux-g++ {
    PRE_TARGETDEPS += $$LIBUI_DIR/libtupigui.so
}

