CONFIG += release

DEFINES += K_DEBUG HAVE_LIBAV
#DEFINES += HAVE_LIBAV

DEFINES += VERSION=\\\"0.2\\\" CODE_NAME=\\\"Arara\\\" REVISION=\\\"09\\\" CONFIG_VERSION=\\\"2\\\"

contains(DEFINES, K_DEBUG) {
    CONFIG += console
}
