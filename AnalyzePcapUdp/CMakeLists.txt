cmake_minimum_required(VERSION 3.8)

project(AnalyzePcapUdp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AUTOMOC ON)
set(AUTORCC ON)
set(AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC
    main.cpp
    networker.cpp
    pcapfile.cpp
    mainwindow.ui
    mainwindow.cpp
    udpworker.cpp
    networker.h
    pcapfile.h
    mainwindow.h
    udpworker.h
    parserfactory.h
    parserfactory.cpp
    pcapngparser.h
    pcapngparser.cpp
    pcapparser.h
    pcapparser.cpp
    assets.qrc
    )

qt6_wrap_cpp(SRC mainwindow.h pcapfile.h)
qt6_wrap_ui(SRC mainwindow.ui)

add_definitions(
    "-DLIBPCAP_EXPORTS=__declspec(dllimport)"
    "-DLIBPACKET_EXPORTS=__declspec(dllimport)"
    )

include_directories(
    ${CMAKE_BINARY_DIR}
    ../3rdparty/winpcap/include
    ../3rdparty/winpcap/include/pcap/
    ../3rdparty/winpcap/libpcap
    ../3rdparty/winpcap/libpcap/Win32/include
    ../3rdparty/libpcapng/include
    )

add_executable(AnalyzePcapUdp ${SRC})

target_link_libraries(AnalyzePcapUdp PRIVATE
    Qt6::Network Qt6::Widgets
    winpcap libpcapng)

set_target_properties(AnalyzePcapUdp
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    )

execute_process(COMMAND "windeployqt" "AnalyzePcapUdp"
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
