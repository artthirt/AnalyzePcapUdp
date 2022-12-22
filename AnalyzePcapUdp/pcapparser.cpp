#include "pcapparser.h"

extern "C"{
#include "pcap.h"
}

class PrivPCapParser{
public:
    pcap_t *mFP = nullptr;
    QString mFileName;

    ~PrivPCapParser(){

    }
    void close(){
        if(mFP){
            pcap_close(mFP);
            mFP = nullptr;
        }
    }

    QString getFileName() const{
        return mFileName;
    }

    void preparePcap()
    {
        u_int netmask = 0xffffff;
        char packet_filter[] = "ip and udp";
        struct bpf_program fcode;

        //compile the filter
        if (pcap_compile(mFP, &fcode, packet_filter, 1, netmask) < 0 )
        {
            fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
            /* Free the device list */
            return;
        }

        //set the filter
        if (pcap_setfilter(mFP, &fcode)<0)
        {
            fprintf(stderr,"\nError setting the filter.\n");
            /* Free the device list */
            return;
        }
    }

    bool open(const QString &fileName)
    {
        close();

        char* errbuf[PCAP_ERRBUF_SIZE];

        mFP = pcap_open_offline(fileName.toLocal8Bit().data(), (char*)errbuf);

        if(mFP == nullptr)
            return false;

        mFileName = fileName;

        preparePcap();

        return true;
    }

    int next_packet(Pkt& pkt)
    {
        if(!mFP)
            return -1;
        struct pcap_pkthdr *_header;
        const u_char *_pkt_data;

        int res = pcap_next_ex(mFP, &_header, &_pkt_data);
        if(res >= 0){
            pkt.caplen = _header->caplen;
            pkt.len = _header->len;
            pkt.ts = _header->ts;
            pkt.data.resize(pkt.len);
            std::copy(_pkt_data, _pkt_data + pkt.len, pkt.data.data());
        }
        return res;
    }

    float position()
    {
#ifdef _MSC_VER
        return pcap_offline_position(mFP);
#else
        return 0;
#endif
    }
};

/////////////////////////////////////

PCapParser::PCapParser()
    :mD(new PrivPCapParser)
{

}

PCapParser::~PCapParser()
{
    mD.reset();
}

bool PCapParser::open(const QString &fileName)
{
    return mD->open(fileName);
}

int PCapParser::next_packet(Pkt &pkt)
{
    return mD->next_packet(pkt);
}

void PCapParser::close()
{
    return mD->close();
}

float PCapParser::position()
{
    return mD->position();
}


bool PCapParser::canOpen(const QString &fileName)
{
    return fileName.toLower().endsWith(".pcap");
}

QString PCapParser::getFileName() const
{
    return mD->getFileName();
}
