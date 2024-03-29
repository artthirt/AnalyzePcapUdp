cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME PCAPPlayer)

project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AUTOMOC ON)
set(AUTORCC ON)
set(AUTOUIC ON)

if(WIN32)
    find_package(Qt6 COMPONENTS Widgets Network)
    if (NOT Qt6_FOUND)
        find_package(Qt5 5.15 COMPONENTS Core Widgets Network REQUIRED)
    endif()
else()
    find_package(Qt5 5 COMPONENTS Core Widgets Network REQUIRED)
    set(Qt6_FOUND OFF)
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

if (NOT Qt6_FOUND)
    qt5_wrap_cpp(SRC mainwindow.h pcapfile.h)
    qt5_wrap_ui(SRC mainwindow.ui)
else()
    qt6_wrap_cpp(SRC mainwindow.h pcapfile.h)
    qt6_wrap_ui(SRC mainwindow.ui)
endif()


if(WIN32)
    add_definitions(
        "-DLIBPCAP_EXPORTS="
        "-DLIBPACKET_EXPORTS="
        )
else()
    add_definitions(
        "-DLIBPCAP_EXPORTS="
        "-DLIBPACKET_EXPORTS="
        )
endif()

set(LIBS)

if(WIN32)
    include_directories(
        ${CMAKE_BINARY_DIR}
        ../3rdparty/winpcap/include
        ../3rdparty/winpcap/include/pcap/
        ../3rdparty/winpcap/libpcap
        ../3rdparty/winpcap/libpcap/Win32/include
        ../3rdparty/libpcapng/include
        )
    set(LIBS ${LIBS} winpcap)
else()
    include_directories(
        ${CMAKE_BINARY_DIR}
        ../3rdparty/libpcapng/include
        )
    set(LIBS ${LIBS} pcap)
endif()

add_executable(${PROJECT_NAME} WIN32 ${SRC} resource.rc)

if (NOT Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core Qt5::Network Qt5::Widgets
        ${LIBS} libpcapng)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt::Core Qt::Network Qt::Widgets
        ${LIBS} libpcapng)
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
    )

#execute_process(COMMAND "windeployqt" "${PROJECT_NAME}"
#    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

#set(QT_DIR "${_qt_cmake_dir}/../../plugins/platforms")

#file(GLOB DLL_FILES "${QT_DIR}/*.*")
#file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
#file(MAKE_DIRECTORY bin\\platforms)

#foreach(IT ${DLL_FILES})
#    message("info: " ${IT})
#    add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            ${IT}
#            $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/
#    )
#endforeach()

if(WIN32)
    if(Qt6_FOUND)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::Widgets>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::Core>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::Network>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::Gui>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/libpcapng/libpcapng.dll
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::QWindowsVistaStylePlugin>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles/
        )
    else()
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles/
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt5::Widgets>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt5::Core>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt5::Network>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt5::Gui>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt5::QWindowsVistaStylePlugin>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles/
        )
    endif()
endif()

