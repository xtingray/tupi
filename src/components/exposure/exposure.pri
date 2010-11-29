
INCLUDEPATH += $$EXPOSURE_DIR

LIBS += -L$$EXPOSURE_DIR -lexposure

linux-g++ {
PRE_TARGETDEPS += $$EXPOSURE_DIR/libexposure.a
}

