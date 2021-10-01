#ifndef PCAPFILE_H
#define PCAPFILE_H

#include <QObject>
#include <QString>
#include <QUdpSocket>
#include <QScopedPointer>
#include <QThread>
#include <QList>
#include <QMap>

extern "C"{
#include "pcap.h"
}

struct Filter{
	ushort dstPort;
	QHostAddress sndHost;
	ushort sndPort;
};

struct IPF{
	QByteArray buffer;
	ushort sport;
	ushort dport;
    uint64_t timestamp = 0;

	IPF(){
		sport = 0;
		dport = 0;
	}
	IPF(const QByteArray &data){
		this->buffer = data;
	}
	void add(ushort offset, const QByteArray& data){
		if(buffer.size() < data.size() + offset){
			buffer.resize(offset + data.size());
		}
		memcpy(buffer.data() + offset, data.data(), data.size());
	}
    uint64_t fromTimeval(const timeval& v){
        timestamp = v.tv_sec * 1000000 + v.tv_usec;
        return timestamp;
    }

    static uint64_t sfromTimeval(const timeval& v){
        auto timestamp = v.tv_sec * 1000000 + v.tv_usec;
        return timestamp;
    }
};

class PCapFile: public QObject{
	Q_OBJECT
public:
    enum TimeoutType {NS, US, MS};
    enum TOP{ETHERNET_FRAME, SLL};

	PCapFile();
	~PCapFile();

	void openFile(const QString& fileName);
	void closeFile();

	void start();
    void stop();
	void pause();

    void setRepeat(bool b);

	bool isPause() const;

	void setFilter(const QMap<ushort, Filter>& filters);

    void setTimeout(qint64 val);

    size_t packetsCount() const { return mNum; }

	void getpacket(const struct pcap_pkthdr *header,
				   const u_char *pkt_data);

    TimeoutType timeoutType() const     { return mTimeoutType; }
    void setTimeoutType(TimeoutType tp) { mTimeoutType = tp; }

    /**
     * @brief setTypeOfPCap
     * @param val
     * ethernet frame - header 14 bytes
     * SLL            - header 16 bytes
     */
    void setTypeOfPCap(TOP val) { mTypeOfPCap = val; }

signals:
    void sendPacketString(quint64 num, quint64 timestamp, uint id, uint size, QString);

private:
	pcap_t *mFP = nullptr;
    QString mFileName;
    quint64 mNum = 0;
    quint64 mBeginTimestamp = 0;
    quint64 mPrevTimestamp = 0;

    TOP mTypeOfPCap = ETHERNET_FRAME;

    bool mStarted = false;
	bool mPause = false;
    bool mRepeat = false;

	bool mDone = false;
	QScopedPointer<QThread> mThread;
	QMap<ushort, Filter> mFilters;

	QMap<int, IPF> mFragments;

	QScopedPointer<QUdpSocket> socket;

	ushort mSrcPort = 0;
	ushort mDstPort = 0;

	ushort ID = 0;
	QByteArray buffer;
	bool isCurrentPort = false;

    qint64 mTimeout = 32;
    TimeoutType mTimeoutType = MS;

    void sendToPort(const Filter &flt, quint64 deltatime);
	void internalStart();
    void preparePcap();
    void openFile();

};


#endif // PCAPFILE_H
