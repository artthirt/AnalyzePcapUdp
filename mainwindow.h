#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QScopedPointer>
#include <QStandardItemModel>
#include <QTimer>

#include "pcapfile.h"
#include "networker.h"

struct udpdata{
    quint64 num = 0;
    quint64 timestamp = 0;
    uint id = 0;
    QString val;
    uint size = 0;

    udpdata(){
        num = id = 0;
    }
    udpdata(quint64 n, quint64 t, uint id, uint size, const QString &v){
        num = n;
        timestamp = t;
        this->id = id;
        val = v;
        this->size = size;
    }
    QList<QStandardItem*> get(){
        QList<QStandardItem*> ret;
        ret.push_back(new QStandardItem(QString::number(num)));
        ret.push_back(new QStandardItem(QString::number(1. * timestamp / 1e+6, 'f', 6)));
        ret.push_back(new QStandardItem("ID " + QString::number(id)));
        ret.push_back(new QStandardItem(QString::number(size)));
        ret.push_back(new QStandardItem(val));
        return ret;
    }
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_pbSelect_clicked();

	void on_pbStart_clicked();

    void onReceivePacketString(quint64 num, quint64 timestamp, uint id, uint size, const QString& val);

    void on_actionOpen_triggered();

    void on_pbStop_clicked();

    void onTimeout();

    void on_pbClear_clicked();

    void on_cbUseScrollDown_clicked(bool checked);

	void on_pbPause_clicked();

    void on_sbTimeout_valueChanged(int arg1);

    void on_cbSelectTimeout_currentIndexChanged(int index);

    void on_pbNetStart_clicked();

    void on_pbNetStop_clicked();

    void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;

    bool mUseScroll = false;
	QString mFileName;
    QStandardItemModel mModel;
    QTimer mTimer;
    QList<udpdata> mPackets;

	QScopedPointer<PCapFile> mPCap;

    QScopedPointer<Networker> mNetworker;

    QMap<ushort, Filter> getFilters();

    void updateTimeout();

	void loadSettings();
	void saveSettings();
};

#endif // MAINWINDOW_H
