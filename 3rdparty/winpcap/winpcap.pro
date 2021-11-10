TEMPLATE = lib

#CONFIG += staticlib

DEFINES += LIBPCAP_EXPORTS=__declspec(dllexport) \
    HAVE_ADDRINFO \
    YY_NEVER_INTERACTIVE \
    yylval=pcap_lval \
    HAVE_STRERROR\
    __STDC__\
    INET6\
    SIZEOF_CHAR=1\
    SIZEOF_SHORT=2\
    SIZEOF_INT=4\
    WPCAP\
    HAVE_SNPRINTF\
    HAVE_VSNPRINTF\
    HAVE_REMOTE\
    _U_=\
    _CRT_SECURE_NO_WARNINGS\
    YY_NO_UNISTD_H\
    HAVE_TC_API \
    DEFINES += \"LIBPACKET_EXPORTS=__declspec(dllimport)\"

include(../../common.pri)

INCLUDEPATH += \
    libpcap/ \
    libpcap/bpf/net \
    libpcap/Win32/include/ \
    src/ \
    include/ \
    include/pcap \
    ../packet/include

LIBS += -lWS2_32 -lpacket -lsetupapi -lversion -ladvapi32 -liphlpapi -llegacy_stdio_definitions

HEADERS += \
    libpcap/acconfig.h \
    libpcap/arcnet.h \
    libpcap/atmuni31.h \
    libpcap/bpf/net/bpf.h \
    libpcap/dlpisubs.h \
    libpcap/ethertype.h \
    libpcap/gencode.h \
    libpcap/ieee80211.h \
    libpcap/llc.h \
    libpcap/nlpid.h \
    libpcap/pcap-int.h \
    libpcap/pcap-namedb.h \
    libpcap/pcap-remote.h \
    libpcap/pcap-stdinc.h \
    libpcap/pcap.h \
    libpcap/pcap1.h \
    libpcap/ppp.h \
    libpcap/remote-ext.h \
    libpcap/sockutils.h \
    libpcap/sunatmpos.h \
    libpcap/tokdefs.h

SOURCES += \
    libpcap/Win32/Src/ffs.c \
    libpcap/bpf/net/bpf_filter.c \
    libpcap/bpf_dump.c \
    libpcap/bpf_image.c \
    libpcap/etherent.c \
    #libpcap/fad-getad.c \
    libpcap/fad-win32.c \
    libpcap/gencode.c \
    libpcap/grammar.c \
    libpcap/inet.c \
    libpcap/nametoaddr.c \
    libpcap/optimize.c \
    libpcap/pcap-new.c \
    libpcap/pcap-remote.c \
    libpcap/pcap-tc.c \
    libpcap/pcap-win32.c \
    libpcap/pcap.c \
    libpcap/savefile.c \
    libpcap/scanner.c \
    libpcap/sockutils.c
