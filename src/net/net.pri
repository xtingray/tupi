
INCLUDEPATH += $$NET_DIR

LIBS += -L$$NET_DIR -ltupinet

QT += network

#linux-g++{
#  PRE_TARGETDEPS += $$NET_DIR/libntup.a
#}

