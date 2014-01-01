INSTALLS += target
target.path = /plugins/

HEADERS += apngplugin.h
SOURCES += apngplugin.cpp

OTHER_FILES += apngplugin.json

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiapngplugin

#LIBS += -ltheora -ltheoraenc -ltheoradec -logg

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi
LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../export_config.pri)

!include(../../../../tupiglobal.pri){
    error("Please run configure first")
}
