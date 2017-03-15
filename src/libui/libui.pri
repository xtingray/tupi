PLAYER_DIR = ../components/player
INCLUDEPATH += $$LIBUI_DIR
INCLUDEPATH += $$PLAYER_DIR

unix {
    LIBS += -L$$LIBUI_DIR -ltupigui
}

win32 {
    LIBS += -L$$LIBUI_DIR/release/ -ltupigui
}

linux-g++ {
    PRE_TARGETDEPS += $$LIBUI_DIR/libtupigui.so
}

