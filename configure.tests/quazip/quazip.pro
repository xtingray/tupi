TARGET = quazip 
DEPENDPATH += .
INCLUDEPATH += /usr/include/quazip

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

# Input
SOURCES += main.cpp
LIBS += -lquazip
