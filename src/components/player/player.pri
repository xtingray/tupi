INCLUDEPATH += $$PLAYER_DIR

unix {
    LIBS += -L$$PLAYER_DIR -ltupiplayer
}

win32 {
    LIBS += -L$$PLAYER_DIR/release/ -ltupiplayer
}

linux-g++ {
    PRE_TARGETDEPS += $$PLAYER_DIR/libtupiplayer.so
}

