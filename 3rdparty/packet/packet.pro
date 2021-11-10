TEMPLATE = lib

#CONFIG += staticlib

include(../../common.pri)

DEFINES += \
    HAVE_IPHELPER_API \
    PACKET_DEBUG_LOUD \
    \"IF_LOUD(a)=\" \"DbgPrint(a)=\"\
    _WINDLL
DEFINES += \"LIBPACKET_EXPORTS=__declspec(dllexport)\"
#DEFINES += \"VOID=__declspec(dllexport)\"

INCLUDEPATH += \
    src/ \
    include/

HEADERS += \
    include/Packet32.h \
    src/NpfImExt.h \
    src/Packet32-Int.h \
    #src/WanPacket.h \
    src/debug.h \
    #src/strsafe.h \
    include/DEBUG.H \
    #src/win_bpf.h

SOURCES += \
    src/AdInfo.c \
    src/NpfImExt.c \
    src/Packet32.c \
    #src/WanPacket.cpp \
    #src/win_bpf_filter.c

LIBS += -lUser32 -lKernel32 -lsetupapi -liphlpapi -lws2_32 -ladvapi32 -lversion -llegacy_stdio_definitions
