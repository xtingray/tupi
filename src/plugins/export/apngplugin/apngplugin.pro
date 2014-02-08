INSTALLS += target
target.path = /plugins/

HEADERS += apngplugin.h

SOURCES += apngplugin.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiapngplugin

#LIBS += -ltheora -ltheoraenc -ltheoradec -logg

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../export_config.pri)

!include(../../../../tupiglobal.pri){
    error("Please run configure first")
}
