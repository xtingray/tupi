TEMPLATE = app
CONFIG -= moc
TARGET = opengl 

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

QT += opengl
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
