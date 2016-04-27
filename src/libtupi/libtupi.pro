QT += opengl core gui svg xml network
TEMPLATE = lib
TARGET = tupi

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

INSTALLS += target 
target.path = /lib/

contains("DEFINES", "ADD_HEADERS") {
    INSTALLS += headers
    headers.target = .
    headers.commands = cp *.h $(INSTALL_ROOT)/include/tupi
    headers.path = /include/tupi
}

HEADERS += tupgraphicalgorithm.h \
           # tupglobal.h \
           tuppaintareaproperties.h \
           tupgradientcreator.h \
           tupgradientselector.h \
           tupgradientviewer.h \
           tuppackagehandler.h \
           tupprojectactionbar.h \
           tupbrushmanager.h \
           tupinputdeviceinformation.h \
           tupsvg2qt.h \
           tupitempreview.h \
           tupcompress.h  \
           tupsocketbase.h \
           tupxmlparserbase.h \
           tupproxyitem.h \
           # tupinthash.h \
           tuppenthicknesswidget.h

SOURCES += tupgraphicalgorithm.cpp \
           tuppaintareaproperties.cpp \
           tupgradientcreator.cpp \
           tupgradientselector.cpp \
           tupgradientviewer.cpp \
           tuppackagehandler.cpp \
           tupprojectactionbar.cpp \
           tupbrushmanager.cpp \
           tupinputdeviceinformation.cpp \
           tupsvg2qt.cpp \
           tupitempreview.cpp \
           tupcompress.cpp  \
           tupsocketbase.cpp \
           tupxmlparserbase.cpp \
           tupproxyitem.cpp \
           tuppenthicknesswidget.cpp

FRAMEWORK_DIR = "../framework"
include($$FRAMEWORK_DIR/framework.pri)
