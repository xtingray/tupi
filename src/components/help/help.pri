INCLUDEPATH += $$HELP_DIR

unix {
    LIBS += -L$$HELP_DIR -ltupihelp
}

win32 {
    LIBS += -L$$HELP_DIR/release/ -ltupihelp
}

linux-g++ {
    PRE_TARGETDEPS += $$HELP_DIR/libtupihelp.so
}
