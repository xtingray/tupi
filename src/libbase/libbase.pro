QT += opengl core gui svg xml network

unix { 
    !include(../../tupiglobal.pri) {
        error("Please configure first")
    }
}

win32 {
    LIBS += -llibEGL -llibGLESv2 -lgdi32 -luser32
    include(../../win.pri)
}

INSTALLS += target 
target.path = /lib

contains("DEFINES", "ADD_HEADERS") {
    INSTALLS += headers 
    headers.commands = cp *.h $(INSTALL_ROOT)/include/tupibase
    headers.path = /include/tupibase
}

macx {
    CONFIG += plugin warn_on
}

HEADERS += tupexportinterface.h \
           tupexportpluginobject.h \
           tupfilterinterface.h \
           tupmodulewidgetbase.h \
           tuppluginmanager.h \
           tuptoolinterface.h \
           tuptoolplugin.h  \
           tuppaintareabase.h \
           tuppaintarearotator.h \
           tupgraphicsscene.h \
           tupguideline.h \
           tupanimationrenderer.h \
           tupwebhunter.h

SOURCES += tupexportpluginobject.cpp \
           tupmodulewidgetbase.cpp \
           tuppluginmanager.cpp \
           tuptoolplugin.cpp  \
           tuppaintareabase.cpp \
           tuppaintarearotator.cpp \
           tupgraphicsscene.cpp \
           tupguideline.cpp \
           tupanimationrenderer.cpp \
           tupwebhunter.cpp

*:!macx{
    CONFIG += dll warn_on
}
TEMPLATE = lib
TARGET = tupibase

FRAMEWORK_DIR = "../framework"
include($$FRAMEWORK_DIR/framework.pri)
LIBTUPI_DIR = ../libtupi
include($$LIBTUPI_DIR/libtupi.pri)
STORE_DIR = ../store
include($$STORE_DIR/store.pri)
