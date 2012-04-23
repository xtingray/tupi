INSTALLS += target
target.path = /plugins/

macx {
    CONFIG += plugin

    INSTALLS += otool
    otool.path = /lib/
    otool.extra  += ../../../../tools/update_dylib_path.rb $(INSTALL_ROOT)/lib/$(TARGET) '^/lib' '$(INSTALL_ROOT)/lib'
}

HEADERS += theoramoviegenerator.h \
           theoraplugin.h

SOURCES += theoramoviegenerator.cpp \
           theoraplugin.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = theoraplugin

LIBS += -ltheora -ltheoraenc -ltheoradec -logg

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../export_config.pri)

!include(../../../../tupiglobal.pri){
    error("Please run configure first")
}
