INCLUDEPATH += $$IMPORT_DIR
LIBS += -L$$IMPORT_DIR -limport

linux-g++ {
    PRE_TARGETDEPS += $$IMPORT_DIR/libimport.so
}
