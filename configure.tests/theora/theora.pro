TEMPLATE = app
CONFIG -= moc
TARGET = theora

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib
}

LIBS += -ltheora -ltheoraenc -ltheoradec -logg

# Input
SOURCES += main.cpp
