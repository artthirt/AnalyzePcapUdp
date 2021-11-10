CONFIG(debug, debug|release){
    DESTDIR = $$PWD/AnalyzePcapUdpBin/debug/
}else{
    DESTDIR = $$PWD/AnalyzePcapUdpBin/release/
}

LIBS += -L$$DESTDIR
