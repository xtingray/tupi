CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupismileplugin

INSTALLS += target 
target.path = /plugins/ 

macx {
    CONFIG += plugin

    INSTALLS += otool
    otool.path = /lib/
    otool.extra  += ../../../../tools/update_dylib_path.rb $(INSTALL_ROOT)/lib/$(TARGET) '^/lib' '$(INSTALL_ROOT)/lib'
}

HEADERS += smileplugin.h
SOURCES += smileplugin.cpp

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi

LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

include(../export_config.pri)

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}

