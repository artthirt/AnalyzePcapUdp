QT += network gui widgets

TARGET = AnalyzePcapUdp
TEMPLATE = app

CONFIG += c++11
CONFIG -= app_bundle

INCLUDEPATH += $$PWD/3rdparty/Include

LIBS += -L$$PWD/3rdparty/Lib/x64 -lPacket -lwpcap -lWS2_32

SOURCES += \
        main.cpp \
    networker.cpp \
    pcapfile.cpp \
    mainwindow.cpp \
    udpworker.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    networker.h \
    pcapfile.h \
    mainwindow.h \
    udpworker.h

FORMS += \
    mainwindow.ui
