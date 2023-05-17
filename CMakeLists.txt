cmake_minimum_required(VERSION 3.8)

project(PCAPPlayer VERSION 1.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/AnalyzePcapUdpBin/debug")
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/AnalyzePcapUdpBin")
endif()

message("--->" ${CMAKE_INSTALL_PREFIX})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})

if(WIN32)
    add_subdirectory(3rdparty/packet)
    add_subdirectory(3rdparty/winpcap)
endif()
add_subdirectory(3rdparty/libpcapng)
add_subdirectory(AnalyzePcapUdp)
