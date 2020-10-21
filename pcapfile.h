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
};

class PCapFile: public QObject{
	Q_OBJECT
public:
	PCapFile();
	~PCapFile();

	void openFile(const QString& fileName);
	void closeFile();

	void start();
    void stop();
	void pause();

	bool isPause() const;

	void setFilter(const QMap<ushort, Filter>& filters);

	void setTimeout(uint val){
		mTimeout = val;
	}

	void getpacket(const struct pcap_pkthdr *header,
				   const u_char *pkt_data);

signals:
    void sendPacketString(quint64 num, uint id, uint size, QString);

private:
	pcap_t *mFP = nullptr;
    QString mFileName;
    quint64 mNum = 0;

    bool mStarted = false;
	bool mPause = false;

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

	uint mTimeout = 32;

	void sendToPort(const Filter &flt);
	void internalStart();
    void openFile();

};


#endif // PCAPFILE_H
