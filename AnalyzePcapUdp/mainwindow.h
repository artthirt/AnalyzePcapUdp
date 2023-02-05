#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nodeinfopackets.h"
#include <QMainWindow>
#include <QDateTime>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QTimer>
#include <QLabel>

#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/GraphicsView>

class QCheckBox;
class QSpinBox;
class QLineEdit;
class QLabel;

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
        QDateTime t = QDateTime::fromMSecsSinceEpoch(timestamp);
        ret.push_back(new QStandardItem(QString::number(1. * timestamp / 1e+3, 'f', 3)
                                        + QString(" (%1").arg(t.toString("yyyy-MM-dd hh:mm:ss:zzz"))));
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
    void onInfoClicked(const QModelIndex &index);
    void onUpdatePackets(const PacketData &data);

    void on_pbClear_clicked();

private:
	Ui::MainWindow *ui;

    std::shared_ptr<QtNodes::DataFlowGraphModel> mModel;
    QtNodes::DataFlowGraphicsScene *mScene;
    QtNodes::GraphicsView *mView;
    NodeInfoPackets* mCurrentInfo = nullptr;
    QStandardItemModel mInfoModel;
    QMap<int, NodeInfoPackets*> mInfoOutputs;

    void updateInfoOutput();
    void changeInfoOutput(int64_t id);

	void loadSettings();
	void saveSettings();
};

#endif // MAINWINDOW_H
