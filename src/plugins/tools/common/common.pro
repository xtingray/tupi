QT += opengl core gui svg xml network
TEMPLATE = lib
TARGET = tupiplugincommon

macx {
    CONFIG += staticlib warn_on
} else {
    CONFIG += dll warn_on
}

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}

win32 {
    include(../../../../win.pri)
}

INSTALLS += target 
target.path = /lib/ 

#headers.target = .
#headers.commands = cp *.h $(INSTALL_ROOT)/include/plugincommon
#headers.path = /include/plugincommon

HEADERS += buttonspanel.h \
           tweenmanager.h \
           stepsviewer.h \
           spinboxdelegate.h \
           target.h

SOURCES += buttonspanel.cpp \
           tweenmanager.cpp \
           stepsviewer.cpp \
           spinboxdelegate.cpp \
           target.cpp
		   
FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

STORE_DIR = ../../../store
LIBTUPI_DIR = ../../../libtupi
LIBBASE_DIR = ../../../libbase

include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$LIBBASE_DIR/libbase.pri)
