INCLUDEPATH += $$PAINTAREA_DIR 
LIBS += -L$$PAINTAREA_DIR -ltupipaintarea 

linux-g++ {
PRE_TARGETDEPS += $$PAINTAREA_DIR/libtupipaintarea.so
}

