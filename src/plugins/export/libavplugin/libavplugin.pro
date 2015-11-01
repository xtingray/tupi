QT += opengl core gui svg xml network

unix {
    !include(../../../../tupiglobal.pri) {
        error("Please run configure first")
    }
}

win32 {
    include(../../../../win.pri)
    include(../../../../libav.win.pri)	
}

mac {
    LIBS += -lavutil
}

INSTALLS += target
target.path = /plugins/

HEADERS += tlibavmoviegenerator.h \
           libavplugin.h

SOURCES += tlibavmoviegenerator.cpp \
           libavplugin.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupilibavplugin

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
# include(../export_config.pri)

LIBBASE_DIR = ../../../libbase
STORE_DIR = ../../../store
LIBTUPI_DIR = ../../../libtupi

include($$LIBBASE_DIR/libbase.pri)
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)

