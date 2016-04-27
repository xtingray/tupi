CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupitheoraplugin

INSTALLS += target
target.path = /plugins/

HEADERS += theoramoviegenerator.h \
           theoraplugin.h

SOURCES += theoramoviegenerator.cpp \
           theoraplugin.cpp

LIBS += -ltheora -ltheoraenc -ltheoradec -logg

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi

LIBS += -L../../../libbase -ltupibase
LIBS += -L../../../store -ltupistore 
LIBS += -L../../../libtupi -ltupi

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

unix {
    !include(../../../../tupiglobal.pri) {
        error("Please run configure first")
    }
}

win32 {
    include(../../../../win.pri)
}
