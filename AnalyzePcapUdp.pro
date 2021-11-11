TEMPLATE = subdirs

win32{
    SUBDIRS += \
        3rdparty/packet \
        3rdparty/winpcap \
        3rdparty/libpcapng \
        AnalyzePcapUdp
}else{
    SUBDIRS += \
        AnalyzePcapUdp
}
