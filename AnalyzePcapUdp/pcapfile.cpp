#include "pcapfile.h"

#include <QTimer>

#include <chrono>
#include <thread>
#include <ostream>

#include "utils.h"

#ifdef Q_OS_LINUX
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

class WaitableTimer
{
public:

    WaitableTimer()
    {
        m_timer = ::CreateWaitableTimer(NULL, FALSE, NULL);
        if (!m_timer)
            throw std::runtime_error("Failed to create waitable time (CreateWaitableTimer), error:" + std::to_string(::GetLastError()));
    }

    ~WaitableTimer()
    {
        ::CloseHandle(m_timer);
        m_timer = NULL;
    }

    void SetAndWait(unsigned relativeTime100Ns)
    {
        LARGE_INTEGER dueTime = { 0 };
        dueTime.QuadPart = static_cast<LONGLONG>(relativeTime100Ns) * -1;

        BOOL res = ::SetWaitableTimer(m_timer, &dueTime, 0, NULL, NULL, FALSE);
        if (!res)
            throw std::runtime_error("SetAndWait: failed set waitable time (SetWaitableTimer), error:" + std::to_string(::GetLastError()));

        DWORD waitRes = ::WaitForSingleObject(m_timer, INFINITE);
        if (waitRes == WAIT_FAILED)
            throw std::runtime_error("SetAndWait: failed wait for waitable time (WaitForSingleObject)" + std::to_string(::GetLastError()));
    }

    void SleepMs(unsigned Ms){
        SetAndWait(Ms * 10000);
    }

private:
    HANDLE m_timer;
};

#endif

#define INLINE_DELAY    0

/* 4 bytes IP address */
typedef union ip_address
{
    u_char byte[4];
    uint32_t ip;
}ip_address;

struct eth_header{
    uchar dst[6];
    uchar src[6];
    ushort type;
};

struct sll_header{
    ushort pkt_type;
    ushort arphrd_type;
    ushort ll_adrlen;
    uchar ll_addr[8];
    ushort ptype;
};

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service
	u_short tlen;			// Total length
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_header;

/* UDP header*/
typedef struct udp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_header;

//void dispatcher_handler(u_char *, const struct pcap_pkthdr *, const u_char *);

ushort Inv(ushort v)
{
    ushort res = (v >> 8) | (v << 8);
    return  res;
}

uint Inv(uint v)
{
	uchar a = v >> 24;
	uchar b = v >> 16;
	uchar c = v >> 8;
	uchar d = v;

	return (uint)((d << 24) | (c << 16) | (b << 8) | a );
}

//void dispatcher_handler(u_char *temp1,
//						const struct pcap_pkthdr *header,
//						const u_char *pkt_data)
//{
//	PCapFile *that = reinterpret_cast<PCapFile*>(temp1);

//	that->getpacket(header, pkt_data);
//}

///////////////////////////////

PCapFile::PCapFile(){

	mThread.reset(new QThread);
	mThread->setObjectName("pcapfile");
	mThread->moveToThread(mThread.data());

	moveToThread(mThread.data());

	mThread->start();
}

PCapFile::~PCapFile()
{
	mDone = true;
    mStarted = false;

	if(mThread.data()){
		mThread->quit();
		mThread->wait();
	}
    closeFile();

}

void PCapFile::openFile(const QString &fileName)
{   
    mFileName = fileName;
}

void PCapFile::closeFile()
{
    if(mParser){
        mParser.reset();
	}
    mFragments.clear();
}

void PCapFile::start()
{
	if(mPause){
		mPause = false;
		return;
	}
	QTimer::singleShot(0, this, [this](){
		internalStart();
    });
}

void PCapFile::stop()
{
    mRepeat = false;
	mStarted = false;
}

void PCapFile::pause()
{
    mPause = true;
}

void PCapFile::setRepeat(bool b)
{
    mRepeat = b;
}

bool PCapFile::isPause() const
{
	return mPause;
}

void PCapFile::setFilter(const QMap<ushort, Filter> &filters)
{
	mFilters = filters;
}

void PCapFile::setTimeout(double val)
{
    mTimeout = val;
}

float PCapFile::position() const
{
    if(mParser)
        return mParser->position();
    return 0;
}

void PCapFile::internalStart()
{
    openFile();

    mStarted = true;

	int res;

    mNum = 0;

    Pkt pkt;

    double timeout = mTimeout;
    double freq = 1000. / timeout;
    auto start = getNow();
    int pkts = 0;
    int pktsInMsCnt = 0;
    double Delay = 1000;
    double pktsInMs = freq / 1000;
    qDebug("Numbers of packet by second %f", freq);
    qDebug("freq %.2f on timeout %.2f, packets in ms %.2f", freq, Delay, pktsInMs);

#ifdef _MSC_VER
    WaitableTimer wTimer;
    timeBeginPeriod(1);
#define SleepX(x) wTimer.SleepMs(x)
#else
#define SleepX(x) std::this_thread::sleep_for(std::chrono::milliseconds(1))
#endif
    do{
        while(!mDone && mStarted){

            if(fabs(timeout - mTimeout) > 1e-16){
                timeout = mTimeout;
                freq = 1000. / timeout;
                pktsInMs = freq / 1000;
            }

            if(mPause){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            if(pktsInMsCnt > pktsInMs){
                pktsInMsCnt = 0;
                SleepX(1);
            }
            if(pkts >= freq){
                double dur = getDuration(start);
                if(dur < Delay){
                    SleepX(1);
                    continue;
                }else{
                    qDebug("packets in %f = %d (%.2f packets/sec)", Delay, pkts, 1000. * pkts / Delay);
                    start = getNow();
                    pkts = 0;
                }
            }

            res = mParser->next_packet(pkt);
            if(res > 0){
                res = getpacket(pkt);
                if(res > 0){
                    pkts++;
                    pktsInMsCnt++;
                }
            }else{
                if( res < 0){
                    break;
                }
            }
        }

        if(mRepeat){
            openFile();
        }
    }while(mRepeat && !mDone);

#ifdef _MSC_VER
    timeEndPeriod(1);
#endif

	socket.reset();

    //pcap_loop(mFP, 0, dispatcher_handler, (u_char*)this);
}

void PCapFile::openFile()
{
    closeFile();

    mParser = ParserFactory::getParser(mFileName);

    if(mParser == nullptr)
        return;

    mParser->open(mFileName);
}

int PCapFile::getpacket(const Pkt pkt)
{
    int ret = 0;
	if(mDone)
        return ret;

	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	udp_header *uh;
	u_int ip_len;
	u_short sport,dport;
    time_t local_tv_sec;
    eth_header *eth;
    sll_header *slh;

    int* begeth = (int*)pkt.data.data();
    eth = (eth_header*) pkt.data.data();
    slh = (sll_header*) pkt.data.data();

    mTypeOfPCap = ETHERNET_FRAME;

    if(begeth[0] == 2){
        ih = (ip_header *) (pkt.data.data() + 4);
    }else{
        if(Inv(slh->pkt_type) >= 0 && Inv(slh->pkt_type) <= 4
                && Inv(slh->arphrd_type) == 1 && Inv(slh->ll_adrlen) <=8){
            mTypeOfPCap = SLL;
        }

        int offtop = mTypeOfPCap == SLL? 16 : 14;
        /* retireve the position of the ip header */
        ih = (ip_header *) (pkt.data.data() +
                            offtop); //length of ethernet header
    }

	/* retireve the position of the udp header */
	ip_len = (ih->ver_ihl & 0xf) * 4;
	uh = (udp_header *) ((u_char*)ih + ip_len);

	ushort off = ntohs(ih->flags_fo);
	uchar Flags = off >> 13;
	off = (off & 0x1fff) * 8;

	bool DF = Flags & 0x2;
	bool MF = Flags & 0x1;

	ushort tlen = ntohs(ih->tlen);

	/* convert from network byte order to host byte order */
	sport = ntohs( uh->sport );
	dport = ntohs( uh->dport );

	if(off == 0){
		mSrcPort = sport;
		mDstPort = dport;
	}

	ID = htons(ih->identification);

    char saddr[30], daddr[30];
    sprintf(saddr, "%d.%d.%d.%d", ih->saddr.byte[0], ih->saddr.byte[1], ih->saddr.byte[2],ih->saddr.byte[3]);
    sprintf(daddr, "%d.%d.%d.%d", ih->daddr.byte[0], ih->daddr.byte[1], ih->daddr.byte[2],ih->daddr.byte[3]);

	int len = 0;
	QByteArray ba;
	len = tlen - ip_len;
	char* offs = (char*)uh;
	ba.append(offs, len);

    if(!mBeginTimestamp){
        mBeginTimestamp = IPF::sfromTimeval(pkt.ts);
    }

    int64_t timestamp = IPF::sfromTimeval(pkt.ts);

	if(!mFilters.empty()){
		if(MF){
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
                mFragments[ID].dstip = ih->daddr.ip;
                mFragments[ID].timestamp = timestamp;
			}
			mFragments[ID].add(off, ba);
        }else if(DF){
			buffer = ba.remove(0, 8);
			if(mFilters.contains(mDstPort)){
				const Filter& flt = mFilters[mDstPort];
                bool send = true;
                if(!flt.dstHost.isNull()){
                    uint ipflt = flt.dstHost.toIPv4Address();
                    send = ipflt == mFragments[ID].dstip;
                }
                if(send){
    //				qDebug("ID %d sport %d dport %d size %d offset %d flags %d len %d",
    //					   ID, mSrcPort, mDstPort, buffer.size(), off, Flags, len);
                    emit sendPacketString(mNum++, timestamp, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d -> %s:%d",
                                                            saddr, daddr, mSrcPort, mDstPort,
                                                            flt.sndHost.toString().toLatin1().data(), flt.sndPort));
                    ret = sendToPort(buffer, flt, timestamp - mPrevTimestamp);

                    mPrevTimestamp = timestamp;
                }
			}
        }else{
			//qDebug("ID %d off %d size %d", ID, off, ba.size());
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
                mFragments[ID].dstip = ih->daddr.ip;
                mFragments[ID].timestamp = timestamp;
			}
			mFragments[ID].add(off, ba);

			buffer = mFragments[ID].buffer.remove(0, 8);

			if(mFilters.contains(mFragments[ID].dport)){
				const Filter& flt = mFilters[mFragments[ID].dport];
                bool send = true;
                if(!flt.dstHost.isNull()){
                    uint ipflt = flt.dstHost.toIPv4Address();
                    send = ipflt == mFragments[ID].dstip;
                }
                if(send){
    //				qDebug("ID %d sport %d dport %d size %d offset %d flags %d len %d",
    //					   ID, mFragments[ID].sport, mFragments[ID].dport, buffer.size(), off, Flags, len);
                    emit sendPacketString(mNum++, mFragments[ID].timestamp, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d -> %s:%d",
                                                            saddr, daddr, mFragments[ID].sport, mFragments[ID].dport,
                                                            flt.sndHost.toString().toLatin1().data(), flt.sndPort));
                    ret = sendToPort(buffer, flt, mFragments[ID].timestamp - mPrevTimestamp);

                    mPrevTimestamp = mFragments[ID].timestamp;
                }
			}

			mFragments.remove(ID);
		}
	}else{
        //qDebug("ID %d sport %d dport %d off %d flags %d size %d", ID, mSrcPort, mDstPort, off, Flags, len);

		if(MF){
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
                mFragments[ID].dstip = ih->daddr.ip;
                mFragments[ID].fromTimeval(pkt.ts);
			}
			mFragments[ID].add(off, ba);
        }else if(DF){
			buffer = ba.remove(0, 8);
            //qDebug("ID %d sport %d dport %d size %d off %d flags %d", ID, mSrcPort, mDstPort, buffer.size(), off, Flags);
            emit sendPacketString(mNum++, timestamp, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d",
                                                    saddr, daddr, mSrcPort, mDstPort));
        }else{
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
                mFragments[ID].dstip = ih->daddr.ip;
                mFragments[ID].fromTimeval(pkt.ts);
			}
			mFragments[ID].add(off, ba);

			buffer = mFragments[ID].buffer.remove(0, 8);

//			qDebug("ID %d sport %d dport %d size %d off %d flags %d size %d",
//				   ID, mFragments[ID].sport, mFragments[ID].dport, buffer.size(), off, Flags, len);
            emit sendPacketString(mNum++, mFragments[ID].timestamp, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d",
                                                    saddr, daddr, mFragments[ID].sport, mFragments[ID].dport));

			mFragments.remove(ID);
		}
	}
    return ret;
}

int PCapFile::sendToPort(const QByteArray& buffer, const Filter& flt, quint64 deltatime)
{
    if(!socket){
		socket.reset(new QUdpSocket);
		socket->moveToThread(mThread.data());
		socket->bind(30001);

        #define SOCKET_BUFFER_SIZE	2 * 1024 * 1024

        int bufLen = SOCKET_BUFFER_SIZE;
        setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char*)&bufLen, sizeof(bufLen));
        setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_SNDBUF, (char*)&bufLen, sizeof(bufLen));

	}

    //qDebug("timedelta %d", deltatime);

	socket->writeDatagram(buffer, flt.sndHost, flt.sndPort);

#if INLINE_DELAY == 1
    auto start = getNow();
    while(getDuration(start) < mTimeout){
        //std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
#endif
    //float delta = 1. * deltatime / 1000;

//    qint64 delay = /*delta * */mTimeout;

//    switch (mTimeoutType) {
//    case NS:
//        std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
//        break;
//    case US:
//        std::this_thread::sleep_for(std::chrono::microseconds(delay));
//        break;
//    case MS:
//        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
//        break;
//    }
    return buffer.size();
}
