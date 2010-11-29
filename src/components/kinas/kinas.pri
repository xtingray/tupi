
INCLUDEPATH += $$KINAS_DIR

LIBS += -L$$KINAS_DIR -lkinas

linux-g++ {
	PRE_TARGETDEPS += $$KINAS_DIR/libkinas.a
}

