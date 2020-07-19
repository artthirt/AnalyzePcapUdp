#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	mPCap.reset(new PCapFile);

	connect(mPCap.data(), SIGNAL(sendPacketString(QString)), this, SLOT(onReceivePacketString(QString)), Qt::QueuedConnection);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    mTimer.start(50);

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

	mPCap->start();
}

void MainWindow::onReceivePacketString(const QString &val)
{
    mPackets << val;
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
    while(!mPackets.empty() && id++ < 20){
        mModel.appendRow( new QStandardItem(mPackets.front()));
        mPackets.pop_front();
    }
    ui->lvOutput->scrollToBottom();
}

void MainWindow::on_pbClear_clicked()
{
    mPackets.clear();
    mModel.clear();
}
