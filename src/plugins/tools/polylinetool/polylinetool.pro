TEMPLATE = lib

INSTALLS += target
target.path = /plugins/
KDEV_QTVER = 4

include(../tools_config.pri)
MOC_DIR = .moc
UI_DIR = .ui
OBJECTS_DIR = .obj

CONFIG += plugin
include(../../../../tupiglobal.pri)
HEADERS += polyline.h \
           infopanel.h

SOURCES += polyline.cpp \
           infopanel.cpp

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
