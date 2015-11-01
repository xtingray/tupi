TEMPLATE = app
CONFIG -= moc
TARGET = multimedia

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

QT += multimedia multimediawidgets
DEPENDPATH += .
INCLUDEPATH += /usr/include/qt5/QtMultimedia /usr/include/qt5/QtMultimediaWidgets

# Input
SOURCES += main.cpp
