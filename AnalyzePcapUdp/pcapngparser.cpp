#include "pcapngparser.h"

extern "C"{
#include <light_pcapng_ext.h>
}

class PrivPCapNg{
public:
    light_pcapng_t *mFP = nullptr;
    QString mFileName;

    ~PrivPCapNg(){
        close();
    }

    bool open(const QString &fileName){
        mFP = light_pcapng_open_read(fileName.toLocal8Bit().data(), LIGHT_TRUE);
        mFileName = fileName;
        return mFP != nullptr;
    }
    QString getFileName() const{
        return mFileName;
    }
    int next_packet(Pkt &pkt){
        if(!mFP)
            return -1;

        struct _light_packet_header ph;
        const uint8_t *pd = nullptr;
        int res = light_get_next_packet(mFP, &ph, &pd);
        if(res > 0){
            const ushort ps = *reinterpret_cast<const ushort*>(&pd[12]);
            if(ps != 8 || ph.original_length < 0x17) /// if packet length less then positon of protocol type
                return 0;
            uchar pt = pd[0x17];
            if(pt != 0x11)  /// only udp
                return 0;
            //printf("%d", ps);

            pkt.ts = ph.timestamp;
            pkt.caplen = ph.captured_length;
            pkt.len = ph.original_length;
            pkt.data.resize(pkt.len);
            std::copy(pd, pd + pkt.len, pkt.data.data());
        }else{
            return -1;
        }
        return res;
    }
    void close(){
        if(!mFP){
            return;
        }
        light_pcapng_close(mFP);
        mFP = nullptr;
    }
    float position(){
        uint64_t pos = 0, size = 1;
        light_get_packet_position(mFP, &pos, &size);
        return 1.f * pos / size;
        return 0;
    }
};

///////////////////////////

PCapNGParser::PCapNGParser()
    : mD(new PrivPCapNg)
{

}

PCapNGParser::~PCapNGParser()
{
    mD.reset();
}

bool PCapNGParser::open(const QString &fileName)
{
    return mD->open(fileName);
}

int PCapNGParser::next_packet(Pkt &pkt)
{
    return mD->next_packet(pkt);
}

void PCapNGParser::close()
{
    mD->close();
}

float PCapNGParser::position()
{
    return mD->position();
}

bool PCapNGParser::canOpen(const QString &fileName)
{
    return fileName.toLower().endsWith(".pcapng");
}

QString PCapNGParser::getFileName() const
{
    return mD->getFileName();
}
