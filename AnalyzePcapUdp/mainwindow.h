#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QTimer>
#include <QLabel>

#include "pcapfile.h"
#include "networker.h"

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

struct UiFilter{
    using shared_label = std::shared_ptr<QLabel>;

    std::shared_ptr<QCheckBox> chk;
    std::shared_ptr<QLineEdit> dstIp;
    std::shared_ptr<QSpinBox> dstPort;
    std::shared_ptr<QLineEdit> outIp;
    std::shared_ptr<QSpinBox> outPort;

    std::vector< shared_label > labels;
    QLabel* newQLabel(const QString& text){
        shared_label sl{new QLabel(text)};
        labels.push_back(sl);
        return sl.get();
    }
};
struct CfFilter{
    bool use{};
    int filter_port{};
    int port{};
    QString ip{};
    QString dst_ip{};
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

    void on_chbRepeat_clicked(bool checked);

    void on_pbHide_clicked();

    void on_dsbDelay_valueChanged(double arg1);

    void on_sbNumFilters_valueChanged(int arg1);

    void on_pbApply_clicked();

private:
	Ui::MainWindow *ui;

    bool mUseScroll = false;
	QString mFileName;
    QStandardItemModel mModel;
    QTimer mTimer;
    QList<udpdata> mPackets;
    QLabel* mLabStatus1;
    QLabel* mLabStatus10;
    QLabel* mLabStatus2;
    QLabel* mLabStatus3;

	QScopedPointer<PCapFile> mPCap;

    QScopedPointer<Networker> mNetworker;

    QVector<UiFilter> mUiFilters;
    QVector<CfFilter> mFilters;

    void initUiFilters(int nums);

    QMap<ushort, Filter> getFilters();

    void updateTimeout();

	void loadSettings();
	void saveSettings();
};

#endif // MAINWINDOW_H
