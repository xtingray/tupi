# Generated automatically at Fri Apr 06 23:03:02 -0500 2012! PLEASE DO NOT EDIT!
LIBS += -lz -lavcodec -lavformat -ltupifwgui -ltupifwcore -ltupifwsound
QT += opengl core gui svg xml network
DEFINES += HAVE_ZLIB HAVE_FFMPEG VERSION=\\\"0.1\\\" CODE_NAME=\\\"Gemo\\\" REVISION=\\\"git12\\\" K_DEBUG
unix {
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}
