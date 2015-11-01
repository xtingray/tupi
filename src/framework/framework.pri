unix {
INCLUDEPATH += $$FRAMEWORK_DIR $$FRAMEWORK_DIR/tcore $$FRAMEWORK_DIR/tgui
LIBS += -L$$FRAMEWORK_DIR/tcore -L$$FRAMEWORK_DIR/tgui -ltupifwcore -ltupifwgui
}

win32 {
INCLUDEPATH += $$FRAMEWORK_DIR $$FRAMEWORK_DIR/tcore $$FRAMEWORK_DIR/tgui
LIBS += -L$$FRAMEWORK_DIR/tcore/release/ -L$$FRAMEWORK_DIR/tgui/release/ -ltupifwcore -ltupifwgui
}