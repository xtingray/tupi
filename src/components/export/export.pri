
INCLUDEPATH += $$EXPORT_DIR

LIBS += -L$$EXPORT_DIR -lexport


linux-g++ {
PRE_TARGETDEPS += $$EXPORT_DIR/libexport.a
}
