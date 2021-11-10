cmake_minimum_required(VERSION 3.8)

project(packet VERSION 1.0)

set(SRC
    include/Packet32.h
    src/NpfImExt.h
    src/Packet32-Int.h
    #src/WanPacket.h
    src/debug.h
    #src/strsafe.h
    include/DEBUG.H
    #src/win_bpf.h
    src/AdInfo.c
    src/NpfImExt.c
    src/Packet32.c
    #src/WanPacket.cpp \
    #src/win_bpf_filter.c
    )

add_definitions("-DLIBPACKET_EXPORTS=__declspec(dllexport)"
    -DHAVE_IPHELPER_API
    -DPACKET_DEBUG_LOUD)

include_directories(src/
    include/)

add_library(packet SHARED ${SRC})

target_link_libraries(packet user32.lib
    kernel32.lib
    ws2_32.lib
    setupapi.lib
    iphlpapi.lib
    advapi32.lib
    version.lib
    legacy_stdio_definitions.lib
    )

message("--->" ${CMAKE_INSTALL_PREFIX})

set_target_properties(packet
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    )
