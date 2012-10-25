INCLUDEPATH += $$ANIMATION_DIR
LIBS += -L$$ANIMATION_DIR -ltupianimation

linux-g++ {
    PRE_TARGETDEPS += $$ANIMATION_DIR/libtupianimation.so
}

