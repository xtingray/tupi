QT += opengl core gui svg xml network

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}

win32 {
    include(../../../../win.pri)
}

INSTALLS += target 
target.path = /plugins/

HEADERS += mouthtarget.h \
           settings.h \
           buttonspanel.h \
           configurator.h \
           lipsyncmanager.h \
           papagayotool.h 

SOURCES += mouthtarget.cpp \
           settings.cpp \
           buttonspanel.cpp \
           configurator.cpp \
           lipsyncmanager.cpp \
           papagayotool.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupipapagayotool

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

LIBBASE_DIR = ../../../libbase
STORE_DIR = ../../../store
LIBTUPI_DIR = ../../../libtupi
# COMMON_DIR = ../common

include($$LIBBASE_DIR/libbase.pri)	
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
# include($$COMMON_DIR/common.pri)
