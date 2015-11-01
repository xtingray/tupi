TEMPLATE = app
CONFIG -= moc
TARGET = zlib

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lz

# Input
SOURCES += main.cpp
