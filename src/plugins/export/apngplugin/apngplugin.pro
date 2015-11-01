unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}

INSTALLS += target
target.path = /plugins/

HEADERS += apngplugin.h
SOURCES += apngplugin.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiapngplugin

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi
LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../export_config.pri)
