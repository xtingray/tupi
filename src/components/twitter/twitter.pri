INCLUDEPATH += $$TWITTER_DIR

unix {
    LIBS += -L$$TWITTER_DIR -ltupitwitter
}

win32 {
    LIBS += -L$$TWITTER_DIR/release/ -ltupitwitter
}

linux-g++ {
    PRE_TARGETDEPS += $$TWITTER_DIR/libtupitwitter.so
}

