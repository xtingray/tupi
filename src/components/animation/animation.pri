INCLUDEPATH += $$ANIMATION_DIR
LIBS += -L$$ANIMATION_DIR -lanimation

linux-g++ {
    PRE_TARGETDEPS += $$ANIMATION_DIR/libanimation.so
}

