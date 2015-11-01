INCLUDEPATH += $$EXPORT_DIR

unix {
    LIBS += -L$$EXPORT_DIR -ltupiexport
}

win32 {
    LIBS += -L$$EXPORT_DIR/release/ -ltupiexport
}

linux-g++ {
    PRE_TARGETDEPS += $$EXPORT_DIR/libtupiexport.so
}
