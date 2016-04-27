CONFIG += warn_on plugin
TEMPLATE = lib
TARGET = tgstengine

!include(../../tupconfig.pri){
    error("Please configure first")
}

INSTALLS += target 
target.path = /lib/plugins/ 

INCLUDEPATH += ../../ ../../tcore
LIBS += -L../../tcore -ltupifwcore

contains(DEFINES, HAVE_GST10){
    HEADERS += tgstengine.h 
    SOURCES += tgstengine.cpp 
}

linux-g{
    TARGETDEPS += ../../tcore/libtupifwcore.so
}
