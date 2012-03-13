# Generated automatically at Mon Mar 12 19:45:09 -0500 2012! PLEASE DO NOT EDIT!
LIBS += -lz -laspell -lavcodec -lavformat -ltupifwgui -ltupifwcore -ltupifwsound
QT += opengl core gui svg xml network
DEFINES += HAVE_ZLIB HAVE_ASPELL HAVE_FFMPEG VERSION=\\\"0.1\\\" CODE_NAME=\\\"Gemo\\\" REVISION=\\\"git12\\\" K_NODEBUG
unix {
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}
CONFIG += silent
