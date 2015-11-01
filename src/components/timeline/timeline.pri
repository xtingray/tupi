INCLUDEPATH += $$TIMELINE_DIR

unix {
    LIBS += -L$$TIMELINE_DIR -ltupitimeline
}

win32 {
    LIBS += -L$$TIMELINE_DIR\release -ltupitimeline
}

linux-g++ {
    PRE_TARGETDEPS += $$TIMELINE_DIR/libtupitimeline.so
}
