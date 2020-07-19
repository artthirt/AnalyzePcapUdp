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

	void setUseFilterDstPort(bool val);
	void setSendingPort(ushort port);
	void setDstPort(ushort port);

	void setTimeout(uint val){
		mTimeout = val;
	}

	void getpacket(const struct pcap_pkthdr *header,
				   const u_char *pkt_data);

signals:
	void sendPacketString(QString);

private:
	pcap_t *mFP = nullptr;
    QString mFileName;

    bool mStarted = false;

	bool mDone = false;
	QScopedPointer<QThread> mThread;
	bool mUseFilterDstPort = false;

	QMap<int, IPF> mFragments;

	QScopedPointer<QUdpSocket> socket;

	ushort mSrcPort = 0;
	ushort mDstPort = 0;

	ushort ID = 0;
	QByteArray buffer;
	ushort dstPort = 10031;
	bool isCurrentPort = false;

	uint mTimeout = 32;

	ushort sendingPort = 10031;
	QHostAddress sendingHost = QHostAddress("127.0.0.1");

	void sendToPort();
	void internalStart();
    void openFile();

};


#endif // PCAPFILE_H
