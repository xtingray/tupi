INCLUDEPATH += $$EXPOSURE_DIR

unix {
    LIBS += -L$$EXPOSURE_DIR -ltupiexposure
}

win32 {
    LIBS += -L$$EXPOSURE_DIR/release/ -ltupiexposure
}

linux-g++ {
    PRE_TARGETDEPS += $$EXPOSURE_DIR/libtupiexposure.so
}
