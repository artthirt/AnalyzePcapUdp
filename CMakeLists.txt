cmake_minimum_required(VERSION 3.8)

project(AnalyzePcapUdp VERSION 1.0)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/AnalyzePcapUdpBin")
message("--->" ${CMAKE_INSTALL_PREFIX})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nodeeditor)

include(FetchContent)

FetchContent_Declare(
    nodeeditor-git
    GIT_REPOSITORY https://github.com/artthirt/nodeeditor.git
    SOURCE_DIR ${EXTERNAL_INSTALL_LOCATION}
)

set(nodeeditor_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/nodeeditor)

if(WIN32)
    add_subdirectory(3rdparty/packet)
    add_subdirectory(3rdparty/winpcap)
endif()
add_subdirectory(3rdparty/libpcapng)
add_subdirectory(AnalyzePcapUdp)
