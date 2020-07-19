QT += network gui widgets

CONFIG += c++11 console
CONFIG -= app_bundle

INCLUDEPATH += $$PWD/3rdparty/Include

LIBS += -L$$PWD/3rdparty/Lib/x64 -lPacket -lwpcap -lWS2_32

SOURCES += \
        main.cpp \
    pcapfile.cpp \
    mainwindow.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    pcapfile.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
