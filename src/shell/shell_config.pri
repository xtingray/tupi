
QT += xml opengl svg

COLORPALETTE_DIR = ../../src/components/colorpalette
ANIMATION_DIR = ../../src/components/animation
PAINTAREA_DIR = ../../src/components/paintarea
PEN_DIR = ../../src/components/pen
KINAS_DIR = ../../src/components/kinas 
HELP_DIR = ../../src/components/help 
IMPORT_DIR = ../../src/components/import 
EXPORT_DIR = ../../src/components/export 
EXPOSURE_DIR = ../../src/components/exposure 
TIMELINE_DIR = ../../src/components/timeline 
DEBUG_DIR = ../../src/components/debug
LIBRARY_DIR = ../../src/components/library
SCENES_DIR = ../../src/components/scenes 
TWITTER_DIR = ../../src/components/twitter
LIBUI_DIR = ../../src/libui
STORE_DIR = ../../src/store 
LIBTUPI_DIR = ../../src/libtupi
QUAZIP_DIR = ../../3rdparty/quazip 
LIBBASE_DIR = ../../src/libbase
NET_DIR = ../../src/net


include($$COLORPALETTE_DIR/colorpalette.pri)
include($$PAINTAREA_DIR/paintarea.pri)
include($$ANIMATION_DIR/animation.pri)
include($$PEN_DIR/pen.pri)
include($$KINAS_DIR/kinas.pri)
include($$HELP_DIR/help.pri)
include($$IMPORT_DIR/import.pri)
include($$EXPORT_DIR/export.pri)
include($$EXPOSURE_DIR/exposure.pri)
include($$TIMELINE_DIR/timeline.pri)
include($$DEBUG_DIR/debug.pri)
include($$LIBRARY_DIR/library.pri)
include($$SCENES_DIR/scenes.pri)
include($$TWITTER_DIR/twitter.pri)
include($$LIBUI_DIR/libui.pri)
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$QUAZIP_DIR/quazip.pri)
include($$LIBBASE_DIR/libbase.pri)
include($$NET_DIR/net.pri)

