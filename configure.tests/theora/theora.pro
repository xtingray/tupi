TEMPLATE = app
CONFIG -= moc

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

INCLUDEPATH += .

LIBS += -ltheora -ltheoraenc -ltheoradec -logg

SOURCES += main.cpp
