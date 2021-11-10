cmake_minimum_required(VERSION 3.8)

project(winpcap VERSION 1.0)

set(SRC
    libpcap/acconfig.h
    libpcap/arcnet.h
    libpcap/atmuni31.h
    libpcap/bpf/net/bpf.h
    libpcap/dlpisubs.h
    libpcap/ethertype.h
    libpcap/gencode.h
    libpcap/ieee80211.h
    libpcap/llc.h
    libpcap/nlpid.h
    libpcap/pcap-int.h
    libpcap/pcap-namedb.h
    libpcap/pcap-remote.h
    libpcap/pcap-stdinc.h
    libpcap/pcap.h
    libpcap/pcap1.h
    libpcap/ppp.h
    libpcap/remote-ext.h
    libpcap/sockutils.h
    libpcap/sunatmpos.h
    libpcap/tokdefs.h
    libpcap/Win32/Src/ffs.c
    libpcap/bpf/net/bpf_filter.c
    libpcap/bpf_dump.c
    libpcap/bpf_image.c
    libpcap/etherent.c
    #libpcap/fad-getad.c
    libpcap/fad-win32.c
    libpcap/gencode.c
    libpcap/grammar.c
    libpcap/inet.c
    libpcap/nametoaddr.c
    libpcap/optimize.c
    libpcap/pcap-new.c
    libpcap/pcap-remote.c
    libpcap/pcap-tc.c
    libpcap/pcap-win32.c
    libpcap/pcap.c
    libpcap/savefile.c
    libpcap/scanner.c
    libpcap/sockutils.c
    )

include_directories(
    libpcap/
    libpcap/bpf/net
    libpcap/Win32/include/
    src/
    include/
    include/pcap
    ../packet/include
    )

add_definitions(
    "-DLIBPCAP_EXPORTS=__declspec(dllexport)"
    -DHAVE_ADDRINFO
    -DYY_NEVER_INTERACTIVE
    "-Dyylval=pcap_lval"
    -DHAVE_STRERROR
    -D__STDC__
    -DINET6
    "-DSIZEOF_CHAR=1"
    "-DSIZEOF_SHORT=2"
    "-DSIZEOF_INT=4"
    -DWPCAP
    -DHAVE_SNPRINTF
    -DHAVE_VSNPRINTF
    -DHAVE_REMOTE
    -D_U_=
    -D_CRT_SECURE_NO_WARNINGS
    -DYY_NO_UNISTD_H
    -DHAVE_TC_API
    "-DLIBPACKET_EXPORTS=__declspec(dllimport)"
    )

add_library(winpcap SHARED ${SRC})

target_link_libraries(winpcap
    WS2_32 packet setupapi version advapi32 iphlpapi legacy_stdio_definitions)

set_target_properties(winpcap
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    )
