CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupischemetool

INSTALLS += target 
target.path = /plugins/

HEADERS += schemetool.h \
           configurator.h

SOURCES += schemetool.cpp \
           configurator.cpp

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

unix {
    !include(../../../../tupiglobal.pri) {
        error("Please run configure first")
    }
}

win32 {
    include(../../../../win.pri)
}
~
