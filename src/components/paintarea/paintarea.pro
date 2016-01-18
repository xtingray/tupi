QT += opengl core gui svg xml network
QT += multimedia multimediawidgets printsupport

INSTALLS += target
target.path = /lib/

INCLUDEPATH += /usr/include/qt5/QtMultimedia /usr/include/qt5/QtMultimediaWidgets

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tuppaintarea.h \
           tupconfigurationarea.h \
           tupdocumentview.h \
           tuppaintareastatus.h \
           tupimagedevice.h \
           tuppaintareacommand.h \
           tuplibrarydialog.h \
           tupcolorwidget.h \
           tupbrushstatus.h \
           tuptoolstatus.h \
           tupcanvas.h \
           tupcanvasview.h \
           tupimagedialog.h \
           tupstoryboarddialog.h \
           tuppendialog.h \
           tuponionopacitydialog.h \
           # tupexposuredialog.h \
           # tupexposurescene.h \
           tuptoolsdialog.h \
           tupinfowidget.h \
           tupruler.h \
           tupcamerainterface.h \
           tupbasiccamerainterface.h \
           tupcameradialog.h \
           tupcamerawindow.h \
           tupvideosurface.h \
           tuppapagayoimporter.h \
           tuppapagayodialog.h

SOURCES += tuppaintarea.cpp \
           tupconfigurationarea.cpp \
           tupdocumentview.cpp \
           tuppaintareastatus.cpp \
           tupimagedevice.cpp \
           tuppaintareacommand.cpp \
           tuplibrarydialog.cpp \
           tupcolorwidget.cpp \
           tupbrushstatus.cpp \
           tuptoolstatus.cpp \
           tupcanvas.cpp \
           tupcanvasview.cpp \
           tupimagedialog.cpp \
           tupstoryboarddialog.cpp \
           tuppendialog.cpp \
           tuponionopacitydialog.cpp \
           # tupexposuredialog.cpp \
           # tupexposurescene.cpp \
           tuptoolsdialog.cpp \
           tupinfowidget.cpp \
           tupruler.cpp \
           tupcamerainterface.cpp \
           tupbasiccamerainterface.cpp \
           tupcameradialog.cpp \
           tupcamerawindow.cpp \
           tupvideosurface.cpp \
           tuppapagayoimporter.cpp \
           tuppapagayodialog.cpp

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupipaintarea

SHELL_DIR = ../../shell/
INCLUDEPATH += $$SHELL_DIR

PLUGIN_DIR = ../../plugins/export/genericexportplugin
INCLUDEPATH += $$PLUGIN_DIR

SELECTION_DIR = ../../plugins/tools/selection
INCLUDEPATH += $$SELECTION_DIR

POLYLINE_DIR = ../../plugins/tools/polyline
INCLUDEPATH += $$POLYLINE_DIR

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
