# TEMPLATE = app
TARGET += 
DEPENDPATH += .
INCLUDEPATH += .

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

# Input
SOURCES += main.cpp
LIBS += -lz
