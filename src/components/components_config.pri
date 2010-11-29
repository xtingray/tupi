
STORE_DIR = ../../../src/store 
LIBTUPI_DIR = ../../../src/libtupi
LIBBASE_DIR = ../../../src/libbase
#KLIB_DIR = ../../../src/klib
#EXPORT_DIR = ../../../src/components/export

include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$LIBBASE_DIR/libbase.pri)
#include($$KLIB_DIR/klib.pri)
#include($$EXPORT_DIR/export.pri)

include(../../../tupiglobal.pri)

INCLUDEPATH += ../../../src/shell
INCLUDEPATH += ../../../src/components/export

QT += xml

