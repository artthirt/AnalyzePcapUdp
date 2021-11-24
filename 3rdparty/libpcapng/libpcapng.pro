TEMPLATE = lib

include(../../common.pri)

win32{
    DEFINES += \"LIBPACKET_EXPORTS=__declspec(dllexport)\"
}else{
    DEFINES += \"LIBPACKET_EXPORTS=__attribute__((visibility(\\\"default\\\")))\"
}

INCLUDEPATH += src \
               include

SOURCES += \
    src/light_advanced.c \
    src/light_alloc.c \
    src/light_internal.c \
    src/light_io.c \
    src/light_manipulate.c \
    src/light_option.c \
    src/light_pcapng.c \
    src/light_pcapng_cont.c \
    src/light_pcapng_ext.c \
    src/light_platform.c

HEADERS += \
    include/light_debug.h \
    include/light_internal.h \
    include/light_pcapng.h \
    include/light_pcapng_ext.h \
    include/light_platform.h \
    include/light_special.h \
    include/light_types.h \
    include/light_util.h
