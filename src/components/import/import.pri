INCLUDEPATH += $$IMPORT_DIR
LIBS += -L$$IMPORT_DIR -ltupimport

linux-g++ {
    PRE_TARGETDEPS += $$IMPORT_DIR/libtupimport.so
}
