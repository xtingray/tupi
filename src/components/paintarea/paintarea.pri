INCLUDEPATH += $$PAINTAREA_DIR 
# $$PLUGIN_DIR
LIBS += -L$$PAINTAREA_DIR -lpaintarea 
# -L$$PLUGIN_DIR -lgenericexportplugin

linux-g++ {
PRE_TARGETDEPS += $$PAINTAREA_DIR/libpaintarea.a
}

