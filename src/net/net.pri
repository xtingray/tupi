# QT += network
INCLUDEPATH += $$NET_DIR

unix {
    LIBS += -L$$NET_DIR -ltupinet
}

win32 {
    LIBS += -L$$NET_DIR/release/ -ltupinet
}


