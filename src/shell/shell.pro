QT += opengl core gui svg xml network
QT += multimedia multimediawidgets printsupport
QT += widgets
CONFIG += warn_on
TEMPLATE = app

macx {
    CONFIG += console static
    ICON = ../../launcher/icons/tupi.icns
    QMAKE_INFO_PLIST = ./Info.plist
    TARGET = ../../bin/Tupi

    INSTALLS += icons

    icons.target = ../../launcher/icons/tupi.png
    icons.commands = cp ../../launcher/icons/tupi.png $(INSTALL_ROOT)/pixmaps
    icons.path = /pixmaps/

    INSTALLS += target \
                tupidata \

    target.path = /bin/

    tupidata.target = data
    tupidata.commands = cp -r data/* $(INSTALL_ROOT)/data
    tupidata.path = /data/
}

unix:!mac {
    INSTALLS += tupidata \
                launcher \
                mime \
                target \
                desktop \
                icons \
                tupiman \
                copyright

    tupidata.target = data
    tupidata.commands = cp -r data/* $(INSTALL_ROOT)/data
    tupidata.path = /data/

    launcher.target = ../../launcher/tupi
    launcher.commands = cp ../../launcher/tupi $(INSTALL_ROOT)/bin; chmod 755 $(INSTALL_ROOT)/bin/tupi
    launcher.path = /bin/

    mime.target = ../../launcher/tupi.xml
    mime.commands = cp ../../launcher/tupi.xml $(INSTALL_ROOT)/share/mime/packages
    mime.path = /share/mime/packages/

    desktop.target = ../../launcher/tupi.desktop
    desktop.commands = cp ../../launcher/tupi.desktop $(INSTALL_ROOT)/applications
    desktop.path = /applications/

    icons.target = ../../launcher/icons/tupi.png
    icons.commands = cp ../../launcher/icons/tupi.png $(INSTALL_ROOT)/pixmaps
    icons.path = /pixmaps/

    tupiman.target = ../components/help/man/tupi.1.gz
    tupiman.commands = cp ../components/help/man/tupi.1.gz $(INSTALL_ROOT)/man1
    tupiman.path = /man1/

    copyright.target = ../components/help/man/copyright
    copyright.commands = cp ../components/help/man/copyright $(INSTALL_ROOT)/share/doc/tupi
    copyright.path = /tupi/

    target.path = /bin/
    TARGET = ../../bin/tupi.bin
}

HEADERS += tupmainwindow.h \
           tupstatusbar.h \
           tupnewproject.h \
           tupapplication.h \
           tuplocalprojectmanagerhandler.h

SOURCES += main.cpp \
           tupmainwindow.cpp \
           tupstatusbar.cpp \
           tupnewproject.cpp \
           tupapplication.cpp \
           tupmainwindow_gui.cpp \
           tuplocalprojectmanagerhandler.cpp

unix {
    HEADERS += tupcrashhandler.h \
               tupcrashwidget.h
    SOURCES += tupcrashhandler.cpp \
               tupcrashwidget.cpp 
}
		   		   
FRAMEWORK_DIR = ../framework
include($$FRAMEWORK_DIR/framework.pri)

unix {
    !include(../../tupiglobal.pri) {
        error("Please run configure first")
    }
}

win32 {
    TARGET = tupi
    include(../../win.pri)
    include(../../quazip.win.pri)
}

include(shell_config.pri)
