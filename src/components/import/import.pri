INCLUDEPATH += $$IMPORT_DIR

unix {
    LIBS += -L$$IMPORT_DIR -ltupimport
}

win32 {
    LIBS += -L$$IMPORT_DIR/release/ -ltupimport
}

linux-g++ {
    PRE_TARGETDEPS += $$IMPORT_DIR/libtupimport.so
}
