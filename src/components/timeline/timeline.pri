
INCLUDEPATH += $$TIMELINE_DIR

LIBS += -L$$TIMELINE_DIR -ltimeline

linux-g++ {
	PRE_TARGETDEPS += $$TIMELINE_DIR/libtimeline.a
}
