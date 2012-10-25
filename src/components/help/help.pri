INCLUDEPATH += $$HELP_DIR
LIBS += -L$$HELP_DIR -ltupihelp

linux-g++ {
    PRE_TARGETDEPS += $$HELP_DIR/libtupihelp.so
}
