INCLUDEPATH += $$TWITTER_DIR
LIBS += -L$$TWITTER_DIR -ltwitter

linux-g++ {
    PRE_TARGETDEPS += $$TWITTER_DIR/libtwitter.so
}

