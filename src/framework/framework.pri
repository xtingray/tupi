
#INCLUDEPATH += $$FRAMEWORK_DIR $$FRAMEWORK_DIR/tcore $$FRAMEWORK_DIR/tgui $$FRAMEWORK_DIR/tsound
INCLUDEPATH += $$FRAMEWORK_DIR $$FRAMEWORK_DIR/tcore $$FRAMEWORK_DIR/tgui

#LIBS += -L$$FRAMEWORK_DIR/tcore -L$$FRAMEWORK_DIR/tgui -L$$FRAMEWORK_DIR/tsound -ltupifwcore -ltupifwgui -ltupifwsound
LIBS += -L$$FRAMEWORK_DIR/tcore -L$$FRAMEWORK_DIR/tgui -ltupifwcore -ltupifwgui
