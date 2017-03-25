QT += opengl core gui svg xml network
TEMPLATE = lib
TARGET = tupifwgui

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

contains(DEFINES, ADD_HEADERS) {
    INSTALLS += headers 
    headers.files += *.h
    headers.path = /include/tupigui
}

HEADERS += taction.h \
           tactionmanager.h \
           tanimwidget.h \
           tapplication.h \
           tbuttonbar.h \
           tcellview.h \
           tcirclebutton.h \
           tcirclebuttonbar.h \
           tclicklineedit.h \
           tcollapsiblewidget.h \
           tcolorbutton.h \
           tcommandhistory.h \
           tconfigurationdialog.h \
           tcontrolnode.h \
           tdoublecombobox.h \
           tdualcolorbutton.h \
           teditspinbox.h \
           tflatbutton.h \
           tfontchooser.h \
           tformfactory.h \
           tformvalidator.h \
           ticon.h \
           # tideality.h \
           timagebutton.h \
           titemselector.h \
           tmainwindow.h \
           tmainwindowabstractsettings.h \
           tmainwindowfactory.h \
           tmoviegenerator.h \
           tmoviegeneratorinterface.h \
           tnodegroup.h \
           tseparator.h \
           toptionaldialog.h \
           tosd.h \
           tpathhelper.h \
           tpushbutton.h \
           tradiobuttongroup.h \
           trulerbase.h \
           tstackedmainwindow.h \
           tstylecombobox.h \
           tabbedmainwindow.h \
           tabdialog.h \
           ttabwidget.h \
           themedocument.h \
           thememanager.h \
           tipdialog.h \
           tupmsgdialog.h \
           ttoolbox.h \
           toolview.h \
           treelistwidget.h \
           treewidgetsearchline.h \
           tvhbox.h \
           tviewbutton.h \
           # twaitstyle.h \
           twidgetlistview.h \
           twizard.h \
           tworkspacemainwindow.h \
           txyspinbox.h \
           tcolorcell.h \
           tslider.h \
           tcolorarrow.xpm \
           tcolorreset.xpm

SOURCES += taction.cpp \
           tactionmanager.cpp \
           tanimwidget.cpp \
           tapplication.cpp \
           tbuttonbar.cpp \
           tcellview.cpp \
           tcirclebutton.cpp \
           tcirclebuttonbar.cpp \
           tclicklineedit.cpp \
           tcolorbutton.cpp \
           tcollapsiblewidget.cpp \
           tcommandhistory.cpp \
           tconfigurationdialog.cpp \
           tcontrolnode.cpp \
           tdoublecombobox.cpp \
           tdualcolorbutton.cpp \
           teditspinbox.cpp \
           tflatbutton.cpp \
           tfontchooser.cpp \
           tformfactory.cpp \
           tformvalidator.cpp \
           ticon.cpp \ 
           timagebutton.cpp \
           titemselector.cpp \
           tmainwindow.cpp \
           tmainwindowfactory.cpp \
           tmoviegenerator.cpp \
           tnodegroup.cpp \
           tseparator.cpp \
           toptionaldialog.cpp \
           tosd.cpp \
           tpathhelper.cpp \
           tpushbutton.cpp \
           tradiobuttongroup.cpp \
           trulerbase.cpp \
           tstackedmainwindow.cpp \
           tstylecombobox.cpp \
           tabbedmainwindow.cpp \
           tabdialog.cpp \
           ttabwidget.cpp \
           themedocument.cpp \
           thememanager.cpp \
           tipdialog.cpp \
           tupmsgdialog.cpp \
           ttoolbox.cpp \
           toolview.cpp \
           treelistwidget.cpp \
           treewidgetsearchline.cpp \
           tvhbox.cpp \
           tviewbutton.cpp \
           # twaitstyle.cpp \
           twidgetlistview.cpp \
           twizard.cpp \
           tworkspacemainwindow.cpp \
           txyspinbox.cpp \
           tcolorcell.cpp \
           tslider.cpp

INCLUDEPATH += ../core ../ ../../libbase

# RESOURCES += tgui_images.qrc

linux-g {
    TARGETDEPS += ../core/libtupifwcore.so
}

unix {
    LIBS += -L../core -ltupifwcore
    # INCLUDEPATH += ../core ../ ../../libbase
    INCLUDEPATH += ../core
}

win32 {
    LIBS += -L../core/release/ -ltupifwcore
    INCLUDEPATH += ../core
}
