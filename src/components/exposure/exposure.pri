INCLUDEPATH += $$EXPOSURE_DIR
LIBS += -L$$EXPOSURE_DIR -ltupiexposure

linux-g++ {
    PRE_TARGETDEPS += $$EXPOSURE_DIR/libtupiexposure.so
}
