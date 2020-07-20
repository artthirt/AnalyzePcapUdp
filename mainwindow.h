#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QScopedPointer>
#include <QStandardItemModel>
#include <QTimer>

#include "pcapfile.h"

struct udpdata{
    quint64 num;
    uint id;
    QString val;

    udpdata(){
        num = id = 0;
    }
    udpdata(quint64 n, uint id, const QString &v){
        num = n;
        this->id = id;
        val = v;
    }
    QList<QStandardItem*> get(){
        QList<QStandardItem*> ret;
        ret.push_back(new QStandardItem(QString::number(num)));
        ret.push_back(new QStandardItem("ID " + QString::number(id)));
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

    void onReceivePacketString(quint64 num, uint id, const QString& val);

    void on_actionOpen_triggered();

    void on_pbStop_clicked();

    void onTimeout();

    void on_pbClear_clicked();

private:
	Ui::MainWindow *ui;

	QString mFileName;
    QStandardItemModel mModel;
    QTimer mTimer;
    QList<udpdata> mPackets;

	QScopedPointer<PCapFile> mPCap;
};

#endif // MAINWINDOW_H
