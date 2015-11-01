INCLUDEPATH += $$COMMON_DIR

unix {
    LIBS += -L$$COMMON_DIR -ltupiplugincommon
}

win32 {
    LIBS += -L$$COMMON_DIR/release/ -ltupiplugincommon
}
