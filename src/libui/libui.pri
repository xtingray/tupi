ANIMATION_DIR = ../components/animation

INCLUDEPATH += $$LIBUI_DIR
INCLUDEPATH += $$ANIMATION_DIR

LIBS += -L$$LIBUI_DIR -lui

linux-g++ {
PRE_TARGETDEPS += $$LIBUI_DIR/libui.a
}

