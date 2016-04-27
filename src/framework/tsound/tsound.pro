TEMPLATE = lib
TARGET = tupifwsound

macx {
    CONFIG += plugin warn_on
} else {
    CONFIG += warn_on dll
}

!include(../tupconfig.pri) {
    error("Run ./configure first")
}

INSTALLS += target
target.path = /lib/

contains("DEFINES", "ADD_HEADERS") {
    INSTALLS += headers 
    headers.files += *.h
    headers.path = /include/tupisound
}

HEADERS += taudioengineiface.h \
           taudioplayer.h
SOURCES += taudioplayer.cpp

INCLUDEPATH += ../tcore ../
LIBS += -L../tcore -ltupifwcore

linux-g{
    TARGETDEPS += ../tcore/libtupifwcore.so
}
