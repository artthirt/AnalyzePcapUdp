TEMPLATE = subdirs

win32{
    message("win32")
    SUBDIRS += \
        3rdparty/packet \
        3rdparty/winpcap \
        3rdparty/libpcapng \
        AnalyzePcapUdp
}else{
    message("linux")
    # need install lipcap-dev
    SUBDIRS += \
        3rdparty/libpcapng \
        AnalyzePcapUdp
}
