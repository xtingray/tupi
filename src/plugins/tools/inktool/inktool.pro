# Subdir relative project main directory: ./src/plugins/tools/inktool
# Target is a library:  

INSTALLS += target 
target.path = /plugins/

HEADERS += inktool.h \
           configurator.h
SOURCES += inktool.cpp \
           configurator.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiinktool

OTHER_FILES += inktool.json

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi
INCLUDEPATH += = ../common
LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi
LIBS += -L../common

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../tools_config.pri)

include(../../../../tupiglobal.pri)
