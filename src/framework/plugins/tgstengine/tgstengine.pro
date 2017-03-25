CONFIG += warn_on plugin
TEMPLATE = lib
TARGET = tgstengine

!include(../../tupconfig.pri){
    error("Please configure first")
}

INSTALLS += target 
target.path = /lib/plugins/ 

INCLUDEPATH += ../../ ../../core
LIBS += -L../../core -ltupifwcore

contains(DEFINES, HAVE_GST10){
    HEADERS += tgstengine.h 
    SOURCES += tgstengine.cpp 
}

linux-g{
    TARGETDEPS += ../../core/libtupifwcore.so
}
