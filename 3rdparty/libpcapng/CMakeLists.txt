cmake_minimum_required(VERSION 3.8)

project(libpcapng VERSION 1.0)

set(SRC
    src/light_advanced.c
    src/light_alloc.c
    src/light_internal.c
    src/light_io.c
    src/light_manipulate.c
    src/light_option.c
    src/light_pcapng.c
    src/light_pcapng_cont.c
    src/light_pcapng_ext.c
    src/light_platform.c
    include/light_debug.h
    include/light_internal.h
    include/light_pcapng.h
    include/light_pcapng_ext.h
    include/light_platform.h
    include/light_special.h
    include/light_types.h
    include/light_util.h
    )

include_directories(src/
    include/)

add_definitions(
    "-DLIBPACKET_EXPORTS=__declspec(dllexport)")

add_library(libpcapng SHARED ${SRC})

#target_link_libraries(libpcapng )

set_target_properties(packet
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    )
