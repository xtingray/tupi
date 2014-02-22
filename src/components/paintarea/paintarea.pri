INCLUDEPATH += /usr/include/QtMobility /usr/include/QtMultimediaKit
INCLUDEPATH += $$PAINTAREA_DIR 
LIBS += -L$$PAINTAREA_DIR -ltupipaintarea -lQtMultimediaKit

linux-g++ {
PRE_TARGETDEPS += $$PAINTAREA_DIR/libtupipaintarea.so
}

