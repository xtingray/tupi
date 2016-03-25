QT += opengl core gui svg xml network

INSTALLS += tpal 

tpal.target = palettes/*.tpal
tpal.commands = test -d $(INSTALL_ROOT)/data/palettes/ || mkdir $(INSTALL_ROOT)/data/palettes; \
                cp palettes/*.tpal $(INSTALL_ROOT)/data/palettes/
tpal.path = /data/palettes/

INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tupcolorpicker.h \
           tupcolorpalette.h \
           tupviewcolorcells.h \
           tupcellscolor.h \
           tuppaletteparser.h \
           tupcolorform.h

SOURCES += tupcolorpicker.cpp \
           tupcolorpalette.cpp \
           tupviewcolorcells.cpp \
           tupcellscolor.cpp \
           tuppaletteparser.cpp \
           tupcolorform.cpp

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupicolorpalette

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)

unix {
    STORE_DIR = ../../store/
    INCLUDEPATH += $$STORE_DIR
    LIBS += -L$$STORE_DIR -ltupistore

    LIBBASE_DIR = ../../libbase/
    INCLUDEPATH += $$LIBBASE_DIR
    LIBS += -L$$LIBBASE_DIR -ltupibase

    LIBTUPI_DIR = ../../libtupi/
    INCLUDEPATH += $$LIBTUPI_DIR
    LIBS += -L$$LIBTUPI_DIR -ltupi

    !include(../../../tupiglobal.pri) {
             error("Run ./configure first!")
    }
}

win32 {
    include(../../../win.pri)

    STORE_DIR = ../../store/
    INCLUDEPATH += $$STORE_DIR
    LIBS += -L$$STORE_DIR/release/ -ltupistore

    LIBBASE_DIR = ../../libbase/
    INCLUDEPATH += $$LIBBASE_DIR
    LIBS += -L$$LIBBASE_DIR/release/ -ltupibase

    LIBTUPI_DIR = ../../libtupi/
    INCLUDEPATH += $$LIBTUPI_DIR
    LIBS += -L$$LIBTUPI_DIR/release/ -ltupi 
}
