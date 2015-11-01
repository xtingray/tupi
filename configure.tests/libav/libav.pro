TEMPLATE = app
CONFIG -= moc
TARGET = libav 

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

INCLUDEPATH += .
DEFINES += __STDC_CONSTANT_MACROS
LIBS += -lavformat -lavcodec -lavutil

# Input
SOURCES += main.cpp
