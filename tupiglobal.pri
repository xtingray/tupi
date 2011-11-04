# Generated automatically at Thu Nov 03 23:42:44 -0500 2011! PLEASE DO NOT EDIT!
LIBS += -laspell -lavcodec -lavformat -ltupifwgui -ltupifwcore -ltupifwsound -lz
QT += opengl core gui svg xml network
DEFINES += HAVE_ASPELL HAVE_FFMPEG VERSION=\\\"0.1\\\" CODE_NAME=\\\"Gemo\\\" REVISION=\\\"git10\\\" K_NODEBUG
unix {
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}
CONFIG += silent
