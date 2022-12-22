#ifndef PCAPFILE_H
#define PCAPFILE_H

#include <QObject>
#include <QString>
#include <QUdpSocket>
#include <QScopedPointer>
#include <QThread>
#include <QList>
#include <QMap>

#include "parserfactory.h"

struct Filter{
    QHostAddress dstHost;
    ushort dstPort;
	QHostAddress sndHost;
	ushort sndPort;
};

struct IPF{
	QByteArray buffer;
	ushort sport;
	ushort dport;
    uint dstip = 0;
    int64_t timestamp = 0;

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
    int64_t fromTimeval(const timeval& v){
        timestamp = (int64_t)v.tv_sec * 1000 + (int64_t)v.tv_usec / 1000;
        return timestamp;
    }

    static int64_t sfromTimeval(const timeval& v){
        int64_t timestamp = (int64_t)v.tv_sec * 1000 + (int64_t)v.tv_usec / 1000;
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

    void setTimeout(double val);

    size_t packetsCount() const { return mNum; }

    float position() const;
    /**
     * @brief getpacket
     * @param pkt
     * @return size of packet if packet sended to network, else 0
     */
    int getpacket(const Pkt pkt);

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
    QSharedPointer<Parser> mParser;
    QString mFileName;
    quint64 mNum = 0;
    qint64 mBeginTimestamp = 0;
    qint64 mPrevTimestamp = 0;
    double mAverageDuration1Ms = 1;

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

    double mTimeout = 32;

    int sendToPort(const QByteArray &buffer, const Filter &flt, quint64 deltatime);
	void internalStart();
    void openFile();

};

void getAverageMsDuration(int count, double desireMs, double& outMs);
void getTimeVals(double mAverageDuration1Ms, double mTimeout, int &DelayMs, int &group_pkts);


#endif // PCAPFILE_H
