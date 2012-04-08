INCLUDEPATH += $$HELP_DIR
LIBS += -L$$HELP_DIR -lhelp

linux-g++ {
    PRE_TARGETDEPS += $$HELP_DIR/libhelp.so
}
