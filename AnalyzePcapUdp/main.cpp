#include <QApplication>

#include <QUdpSocket>

#include <QDateTime>
#include <QDebug>
#include <chrono>
#include <thread>

#include "mainwindow.h"

#include "pcapfile.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    //double ms;
    //getAverageMsDuration(100, 1, ms);
    //qDebug("set 1 ms == %f", ms);
//	QDateTime dtt(QDate(1999, 8, 22), QTime( 0, 0, 0));
//	quint64 ct = 0;
//	QDateTime dt;
//	if(1){
//		ct = dtt.toTime_t() + 0x24ea0000;
//		dt = QDateTime::fromTime_t(ct);
//	}
//	qDebug() << dt;

//	QUdpSocket sock;
//	sock.bind(8090);

//	QByteArray data;

//	while(1){
//		if(sock.hasPendingDatagrams()){
//			int sz = sock.pendingDatagramSize();
//			data.resize(sz);
//			sock.readDatagram(data.data(), data.size());

//			qDebug("size %d", sz);
//		}else{
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		}
//	}

//	PCapFile pcapF("d:\\Down\\smpl\\dir14\\bittwist-win-2.0\\src\\gspvlc250620_1.pcap");

//	pcapF.start();

	MainWindow w;

	w.show();

	return a.exec();
}
