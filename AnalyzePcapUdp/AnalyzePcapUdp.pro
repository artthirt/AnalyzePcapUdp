QT += network gui widgets

TARGET = AnalyzePcapUdp
TEMPLATE = app

CONFIG += c++11
CONFIG -= app_bundle

win32{
    DEFINES += LIBPCAP_EXPORTS=__declspec(dllimport)
    DEFINES += LIBPACKET_EXPORTS=__declspec(dllimport)
}else{
    DEFINES += LIBPCAP_EXPORTS=
    DEFINES += LIBPACKET_EXPORTS=
}

include(../common.pri)

win32{

    INCLUDEPATH += \
        $$PWD/../3rdparty/winpcap/include \
        $$PWD/../3rdparty/winpcap/include/pcap/ \
        $$PWD/../3rdparty/winpcap/libpcap \
        $$PWD/../3rdparty/winpcap/libpcap/Win32/include \
        $$PWD/../3rdparty/libpcapng/include

    LIBS += -L$$PWD/3rdparty/Lib -lwinpcap -llibpcapng -lWS2_32
}else{
    INCLUDEPATH += $$PWD/../3rdparty/libpcapng/include

    LIBS += -lpcap -llibpcapng
}

SOURCES += \
        main.cpp \
    networker.cpp \
    parserfactory.cpp \
    pcapfile.cpp \
    mainwindow.cpp \
    pcapngparser.cpp \
    pcapparser.cpp \
    udpworker.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    networker.h \
    parserfactory.h \
    pcapfile.h \
    mainwindow.h \
    pcapngparser.h \
    pcapparser.h \
    udpworker.h \
    utils.h

FORMS += \
    mainwindow.ui

RC_ICONS = $$PWD\assets\images\icon-ethernet-Freepik_black.ico

MOC_DIR     = tmp/moc
OBJECTS_DIR = tmp/obj
UI_DIR      = tmp/ui
RCC_DIR     = tmp/rcc

RESOURCES += \
    assets.qrc

win32{
    defineTest(deployApp){
        EXT = .exe
        DESTFILE = $$DESTDIR/$$TARGET$$EXT
        DESTFILE=\"$$quote($$shell_path($$DESTFILE))\"
        QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/windeployqt $$DESTFILE $$escape_expand(\\n\\t)
        export(QMAKE_POST_LINK)
    }
    deployApp()
}
