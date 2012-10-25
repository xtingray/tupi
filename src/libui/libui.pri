ANIMATION_DIR = ../components/animation

INCLUDEPATH += $$LIBUI_DIR
INCLUDEPATH += $$ANIMATION_DIR

LIBS += -L$$LIBUI_DIR -ltupigui

linux-g++ {
    PRE_TARGETDEPS += $$LIBUI_DIR/libtupigui.so
}

