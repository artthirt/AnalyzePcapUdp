#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QScopedPointer>
#include <QStandardItemModel>
#include <QTimer>

#include "pcapfile.h"

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

	void onReceivePacketString(const QString& val);

    void on_actionOpen_triggered();

    void on_pbStop_clicked();

    void onTimeout();

    void on_pbClear_clicked();

private:
	Ui::MainWindow *ui;

	QString mFileName;
    QStandardItemModel mModel;
    QTimer mTimer;
    QStringList mPackets;

	QScopedPointer<PCapFile> mPCap;
};

#endif // MAINWINDOW_H
