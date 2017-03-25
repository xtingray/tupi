unix {
INCLUDEPATH += $$FRAMEWORK_DIR $$FRAMEWORK_DIR/core $$FRAMEWORK_DIR/gui
LIBS += -L$$FRAMEWORK_DIR/core -L$$FRAMEWORK_DIR/gui -ltupifwcore -ltupifwgui
}

win32 {
INCLUDEPATH += $$FRAMEWORK_DIR $$FRAMEWORK_DIR/core $$FRAMEWORK_DIR/gui
LIBS += -L$$FRAMEWORK_DIR/core/release/ -L$$FRAMEWORK_DIR/gui/release/ -ltupifwcore -ltupifwgui
}
