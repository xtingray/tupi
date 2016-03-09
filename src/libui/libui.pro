QT += opengl core gui svg xml network

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

macx {
    CONFIG += plugin warn_on
}

# HEADERS += tupthemeselector.h \
HEADERS += tupabout.h \
           tuppaintareaconfig.h \
		   tupgeneralpreferences.h \
           tuppreferencesdialog.h
 
# SOURCES += tupthemeselector.cpp \
SOURCES += tupabout.cpp \
           tuppaintareaconfig.cpp \
           tupgeneralpreferences.cpp \
           tuppreferencesdialog.cpp 

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupigui

STORE_DIR = ../store
LIBTUPI_DIR = ../libtupi

FRAMEWORK_DIR = "../framework"
include($$FRAMEWORK_DIR/framework.pri)
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
