QT += opengl core gui svg xml network
TEMPLATE = lib
TARGET = tupigui

macx {
    CONFIG += plugin warn_on
} else {
    CONFIG += dll warn_on
}

unix {
    !include(../../tupiglobal.pri) {
        error("Please configure first")
    }
}

win32 {
    include(../../quazip.win.pri)
    include(../../win.pri)
}

INSTALLS += images images-en 
images-en.files += images/en/*.png 
images-en.path = /data/en/images 
images.files += images/es/*.png 
images.path = /data/es/images/ 

INSTALLS += target
target.path = /lib/

contains("DEFINES", "ADD_HEADERS") {
    INSTALLS += headers
    headers.target = .
    headers.commands = cp *.h $(INSTALL_ROOT)/include/tupigui
    headers.path = /include/tupigui
}

HEADERS += tupabout.h \
           tupthemepreferences.h \
           tuppaintareapreferences.h \
           tupgeneralpreferences.h \
           tuppreferencesdialog.h
 
SOURCES += tupabout.cpp \
           tupthemepreferences.cpp \
           tuppaintareapreferences.cpp \
           tupgeneralpreferences.cpp \
           tuppreferencesdialog.cpp 

STORE_DIR = ../store
LIBTUPI_DIR = ../libtupi

FRAMEWORK_DIR = "../framework"
include($$FRAMEWORK_DIR/framework.pri)
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
