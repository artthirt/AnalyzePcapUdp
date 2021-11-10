TEMPLATE = subdirs

win32{
    SUBDIRS += \
        3rdparty/packet \
        3rdparty/winpcap \
        AnalyzePcapUdp
}else{
    SUBDIRS += \
        AnalyzePcapUdp
}
