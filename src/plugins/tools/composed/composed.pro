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

HEADERS += tweener.h \
           configurator.h \
           tweenerpanel.h \
           tweenertable.h \
           positionsettings.h

SOURCES += tweener.cpp \
           # buttonspanel.cpp \
           configurator.cpp \
           tweenerpanel.cpp \
           tweenertable.cpp \
           positionsettings.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupicomposedtool

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

LIBBASE_DIR = ../../../libbase
STORE_DIR = ../../../store
LIBTUPI_DIR = ../../../libtupi
COMMON_DIR = ../common

include($$LIBBASE_DIR/libbase.pri)	
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$COMMON_DIR/common.pri)
