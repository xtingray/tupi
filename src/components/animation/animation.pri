INCLUDEPATH += $$ANIMATION_DIR

unix {
    LIBS += -L$$ANIMATION_DIR -ltupianimation
}

win32 {
    LIBS += -L$$ANIMATION_DIR/release/ -ltupianimation
}

linux-g++ {
    PRE_TARGETDEPS += $$ANIMATION_DIR/libtupianimation.so
}

