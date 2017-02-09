INSTALLS += target
target.path = /lib/

CONFIG += dll warn_on

HEADERS += tupseditor.h \
           tupsfunctionview.h \
           kinaswidget.h 
SOURCES += tupseditor.cpp \
           tupsfunctionview.cpp \
           kinaswidget.cpp 

TEMPLATE = lib
TARGET = tupikinas

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)


