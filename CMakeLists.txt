cmake_minimum_required(VERSION 3.8)

project(AnalyzePcapUdp VERSION 1.0)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/AnalyzePcapUdpBin")
message("--->" ${CMAKE_INSTALL_PREFIX})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

add_subdirectory(3rdparty/packet)
add_subdirectory(3rdparty/winpcap)
add_subdirectory(3rdparty/libpcapng)
add_subdirectory(AnalyzePcapUdp)
