QT += opengl core gui svg xml network
TEMPLATE = lib
TARGET = tupifwcore

macx {
    CONFIG += plugin warn_on
} else {
    CONFIG += warn_on dll
}

unix {
    !include(../tupconfig.pri) {
        error("Run ./configure first!")
    }
}

win32 {
    include(../../../win.pri)
}

INSTALLS += target
target.path = /lib/

contains("DEFINES", "ADD_HEADERS") {
    INSTALLS += headers 
    headers.files += *.h
    headers.path = /include/tupicore
}

HEADERS += talgorithm.h \
           tapplicationproperties.h \
           tconfig.h \
           tglobal.h \
           tipdatabase.h \
           txmlparserbase.h

SOURCES += talgorithm.cpp \
           tapplicationproperties.cpp \
           tconfig.cpp \
           tipdatabase.cpp \
           txmlparserbase.cpp

unix {
    contains(DEFINES, K_DEBUG) {
        HEADERS += tdebug.h
        SOURCES += tdebug.cpp
    }
}
