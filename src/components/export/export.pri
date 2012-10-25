INCLUDEPATH += $$EXPORT_DIR
LIBS += -L$$EXPORT_DIR -ltupiexport

linux-g++ {
    PRE_TARGETDEPS += $$EXPORT_DIR/libtupiexport.so
}
