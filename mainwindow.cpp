#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    mPCap.reset(new PCapFile);

    connect(mPCap.data(), SIGNAL(sendPacketString(quint64, uint, QString)),
            this, SLOT(onReceivePacketString(quint64, uint, QString)), Qt::QueuedConnection);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    mTimer.start(50);

    mModel.setHorizontalHeaderLabels(QStringList() << "num" << "id" << "data");

    ui->lvOutput->setModel(&mModel);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pbSelect_clicked()
{
	QString fn = QFileDialog::getOpenFileName(nullptr, "Open File...", "../../", "*.pcap");

	if(fn.isEmpty())
		return;

	mFileName = fn;

	ui->lbSelectedFile->setText(fn);
}

void MainWindow::on_pbStart_clicked()
{
	if(mFileName.isEmpty() || !QFile::exists(mFileName))
		return;

	if(!mPCap.data())
		return;

	mPCap->openFile(mFileName);

	mPCap->setTimeout(ui->sbTimeout->value());
	mPCap->setUseFilterDstPort(ui->chbFilterDstPort->isChecked());
	mPCap->setDstPort(ui->sbFilterDstPort->value());
	mPCap->setSendingPort(ui->sbDstPort->value());
    mPCap->setSendingHost(ui->leIp->text());

    mModel.setHorizontalHeaderLabels(QStringList() << "num" << "id" << "data");
    //ui->lvOutput->setColumnWidth(2, 600);

	mPCap->start();
}

void MainWindow::onReceivePacketString(quint64 num, uint id, const QString &val)
{
    mPackets.push_back(udpdata(num, id, val));
    //ui->pteDebug->appendPlainText(val);
    //mModel.insertRow(mModel.rowCount(), new QStandardItem(val));
}

void MainWindow::on_actionOpen_triggered()
{
    on_pbSelect_clicked();
}

void MainWindow::on_pbStop_clicked()
{
    mPCap->stop();
    mPackets.clear();
}

void MainWindow::onTimeout()
{
    int id = 0;
    while(!mPackets.empty() && id++ < 40){
        mModel.appendRow(mPackets.front().get());
        mPackets.pop_front();
    }
    ui->lvOutput->scrollToBottom();

    ui->statusbar->showMessage("Packets left " +QString::number(mPackets.size()));
}

void MainWindow::on_pbClear_clicked()
{
    mPackets.clear();
    mModel.clear();
}
