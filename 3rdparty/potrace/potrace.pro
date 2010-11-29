# Subdir relative project main directory: ./3rdparty/potrace
# Target is a library:  

HEADERS += auxiliary.h \
           bitmap.h \
           curve.h \
           decompose.h \
           lists.h \
           platform.h \
           potracelib.h \
           progress.h \
           trace.h \
           config.h 
SOURCES += curve.c \
           decompose.c \
           potracelib.c \
           trace.c 
CONFIG += release warn_on staticlib 
TEMPLATE = lib 

