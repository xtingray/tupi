INCLUDEPATH += $$TWITTER_DIR
LIBS += -L$$TWITTER_DIR -ltupitwitter

linux-g++ {
    PRE_TARGETDEPS += $$TWITTER_DIR/libtupitwitter.so
}

