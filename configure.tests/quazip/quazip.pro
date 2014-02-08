TARGET = quazip
DEPENDPATH += .

macx {
    INCLUDEPATH += /usr/local/include/quazip
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

unix:!macx {
  INCLUDEPATH += /usr/include/quazip
}

# Input
SOURCES += main.cpp
LIBS += -lquazip
