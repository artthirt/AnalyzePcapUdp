#include "pcapfile.h"

#include <QTimer>

#include <chrono>
#include <thread>

#include <WinSock2.h>

/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

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

void dispatcher_handler(u_char *, const struct pcap_pkthdr *, const u_char *);

uint Inv(uint v)
{
	uchar a = v >> 24;
	uchar b = v >> 16;
	uchar c = v >> 8;
	uchar d = v;

	return (uint)((d << 24) | (c << 16) | (b << 8) | a );
}

void dispatcher_handler(u_char *temp1,
						const struct pcap_pkthdr *header,
						const u_char *pkt_data)
{
	PCapFile *that = reinterpret_cast<PCapFile*>(temp1);

	that->getpacket(header, pkt_data);
}

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
	if(mFP){
		pcap_close(mFP);
		mFP = nullptr;
	}
    mFragments.clear();
}

void PCapFile::start()
{
	QTimer::singleShot(0, this, [this](){
		internalStart();
    });
}

void PCapFile::stop()
{
    mStarted = false;
}

void PCapFile::internalStart()
{
    openFile();

    mStarted = true;
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

	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	int res;

    mNum = 0;

    while((res = pcap_next_ex(mFP, &header, &pkt_data)) >= 0 && !mDone && mStarted){
		getpacket(header, pkt_data);
	}

	socket.reset();

    //pcap_loop(mFP, 0, dispatcher_handler, (u_char*)this);
}

void PCapFile::openFile()
{
    closeFile();

    char* errbuf[PCAP_ERRBUF_SIZE];

    mFP = pcap_open_offline(mFileName.toLocal8Bit().data(), (char*)errbuf);

    if(mFP == NULL)
        return;
}

void PCapFile::setUseFilterDstPort(bool val)
{
    mUseFilterDstPort = val;
}

void PCapFile::setSendingHost(const QString &ip)
{
    sendingHost = QHostAddress(ip);
}

void PCapFile::setSendingPort(ushort port)
{
	sendingPort = port;
}

void PCapFile::setDstPort(ushort port)
{
	dstPort = port;
}

void PCapFile::getpacket(const pcap_pkthdr *header, const u_char *pkt_data)
{
	if(mDone)
		return;

	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	udp_header *uh;
	u_int ip_len;
	u_short sport,dport;
	time_t local_tv_sec;

	/* retireve the position of the ip header */
	ih = (ip_header *) (pkt_data +
						14); //length of ethernet header

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
    sprintf(saddr, "%d.%d.%d.%d", ih->saddr.byte1, ih->saddr.byte2, ih->saddr.byte3,ih->saddr.byte4);
    sprintf(daddr, "%d.%d.%d.%d", ih->daddr.byte1, ih->daddr.byte2, ih->daddr.byte3,ih->daddr.byte4);

	int len = 0;
	QByteArray ba;
	len = tlen - ip_len;
	char* offs = (char*)uh;
	ba.append(offs, len);

    if(mUseFilterDstPort){
		if(MF){
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
			}
			mFragments[ID].add(off, ba);
        }else if(DF){
			buffer = ba.remove(0, 8);
            if(mDstPort == dstPort){
//				qDebug("ID %d sport %d dport %d size %d offset %d flags %d len %d",
//					   ID, mSrcPort, mDstPort, buffer.size(), off, Flags, len);
                emit sendPacketString(mNum++, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d -> %s:%d",
                                                        saddr, daddr, mSrcPort, mDstPort,
                                                        sendingHost.toString().toLatin1().data(), sendingPort));
				sendToPort();
			}
        }else{
			//qDebug("ID %d off %d size %d", ID, off, ba.size());
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
			}
			mFragments[ID].add(off, ba);

			buffer = mFragments[ID].buffer.remove(0, 8);

			if(mFragments[ID].dport == dstPort){
//				qDebug("ID %d sport %d dport %d size %d offset %d flags %d len %d",
//					   ID, mFragments[ID].sport, mFragments[ID].dport, buffer.size(), off, Flags, len);
                emit sendPacketString(mNum++, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d -> %s:%d",
                                                        saddr, daddr, mFragments[ID].sport, mFragments[ID].dport,
                                                        sendingHost.toString().toLatin1().data(), sendingPort));
				sendToPort();
			}

			mFragments.remove(ID);
		}
	}else{
        //qDebug("ID %d sport %d dport %d off %d flags %d size %d", ID, mSrcPort, mDstPort, off, Flags, len);

		if(MF){
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
			}
			mFragments[ID].add(off, ba);
        }else if(DF){
			buffer = ba.remove(0, 8);
            //qDebug("ID %d sport %d dport %d size %d off %d flags %d", ID, mSrcPort, mDstPort, buffer.size(), off, Flags);
            emit sendPacketString(mNum++, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d",
                                                    saddr, daddr, mSrcPort, mDstPort));
        }else{
			if(!mFragments.contains(ID)){
				mFragments[ID].sport = mSrcPort;
				mFragments[ID].dport = mDstPort;
			}
			mFragments[ID].add(off, ba);

			buffer = mFragments[ID].buffer.remove(0, 8);

//			qDebug("ID %d sport %d dport %d size %d off %d flags %d size %d",
//				   ID, mFragments[ID].sport, mFragments[ID].dport, buffer.size(), off, Flags, len);
            emit sendPacketString(mNum++, ID,  buffer.size(), QString::asprintf("ipsrc %s ipdst %s sport %d dport %d",
                                                    saddr, daddr, mFragments[ID].sport, mFragments[ID].dport));

			mFragments.remove(ID);
		}
	}
}

void PCapFile::sendToPort(){

	if(!socket){
		socket.reset(new QUdpSocket);
		socket->moveToThread(mThread.data());
		socket->bind(30001);

        #define SOCKET_BUFFER_SIZE	1024 * 1024

        int bufLen = SOCKET_BUFFER_SIZE;
        setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char*)&bufLen, sizeof(bufLen));
        setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_SNDBUF, (char*)&bufLen, sizeof(bufLen));

	}

	socket->writeDatagram(buffer, sendingHost, sendingPort);

	std::this_thread::sleep_for(std::chrono::milliseconds(mTimeout));
}
