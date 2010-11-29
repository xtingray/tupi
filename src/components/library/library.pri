
INCLUDEPATH += $$LIBRARY_DIR

LIBS += -L$$LIBRARY_DIR -llibrary

linux-g++ {
PRE_TARGETDEPS += $$LIBRARY_DIR/liblibrary.a
}
