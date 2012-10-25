INCLUDEPATH += $$TIMELINE_DIR
LIBS += -L$$TIMELINE_DIR -ltupitimeline

linux-g++ {
    PRE_TARGETDEPS += $$TIMELINE_DIR/libtupitimeline.so
}
