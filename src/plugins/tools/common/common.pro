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
target.path = /lib/ 

#headers.target = .
#headers.commands = cp *.h $(INSTALL_ROOT)/include/plugincommon
#headers.path = /include/plugincommon

macx {
    CONFIG += staticlib warn_on
}

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
		   
*:!macx {
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupiplugincommon 

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

STORE_DIR = ../../../store
LIBTUPI_DIR = ../../../libtupi
LIBBASE_DIR = ../../../libbase

include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$LIBBASE_DIR/libbase.pri)
