cmake_minimum_required(VERSION 3.8)

project(AnalyzePcapUdp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AUTOMOC ON)
set(AUTORCC ON)
set(AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Network)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Widgets Network REQUIRED)
endif()

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
    Qt::Core Qt::Network Qt::Widgets
    winpcap libpcapng)

set_target_properties(AnalyzePcapUdp
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    )

#execute_process(COMMAND "windeployqt" "AnalyzePcapUdp"
#    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

#set(QT_DIR "${_qt_cmake_dir}/../../plugins/platforms")

#file(GLOB DLL_FILES "${QT_DIR}/*.*")
#file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
#file(MAKE_DIRECTORY bin\\platforms)

#foreach(IT ${DLL_FILES})
#    message("info: " ${IT})
#    add_custom_command(
#        TARGET AnalyzePcapUdp POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            ${IT}
#            $<TARGET_FILE_DIR:AnalyzePcapUdp>/platforms/
#    )
#endforeach()

if(Qt6_FOUND)
    add_custom_command(
        TARGET AnalyzePcapUdp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Widgets>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Network>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/libpcapng/libpcapng.dll
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
    )
else()
    add_custom_command(
        TARGET AnalyzePcapUdp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt5::Widgets>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt5::Core>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt5::Network>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt5::Gui>
            $<TARGET_FILE_DIR:AnalyzePcapUdp>
    )
endif()

