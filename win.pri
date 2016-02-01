CONFIG += release

#DEFINES += K_DEBUG HAVE_LIBAV
DEFINES += HAVE_LIBAV

contains(DEFINES, K_DEBUG) {
    CONFIG += console
}
