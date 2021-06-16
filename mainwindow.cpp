#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    mPCap.reset(new PCapFile);

    connect(mPCap.data(), SIGNAL(sendPacketString(quint64, uint, uint, QString)),
            this, SLOT(onReceivePacketString(quint64, uint, uint, QString)), Qt::QueuedConnection);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    mTimer.start(50);

    mModel.setHorizontalHeaderLabels(QStringList() << "num" << "id" << "size" << "data");

	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 3);

    ui->lvOutput->setModel(&mModel);

	loadSettings();
}

MainWindow::~MainWindow()
{
	saveSettings();

	delete ui;
}

void MainWindow::on_pbSelect_clicked()
{
    QFileInfo fi(mFileName);

    QString fn = QFileDialog::getOpenFileName(nullptr, "Open File...", fi.path(), "*.pcap");

	if(fn.isEmpty())
		return;

	mFileName = fn;

	ui->lbSelectedFile->setText(fn);
}

void setFilter(QMap<ushort, Filter>& filters, QCheckBox* cb, QSpinBox* dstPort, QLineEdit* ip, QSpinBox* dstPort2)
{
	if(cb->isChecked()){
		Filter flt;
		flt.dstPort = dstPort->value();
		flt.sndHost = QHostAddress(ip->text());
		flt.sndPort = dstPort2->value();
		filters[flt.dstPort] = flt;
	}
}

void MainWindow::on_pbStart_clicked()
{
	if(mFileName.isEmpty() || !QFile::exists(mFileName))
		return;

	if(!mPCap.data())
		return;

	mPCap->openFile(mFileName);

	mPCap->setTimeout(ui->sbTimeout->value());

	QMap<ushort, Filter> filters;
	setFilter(filters, ui->chbFilterDstPort, ui->sbFilterDstPort, ui->leIp, ui->sbDstPort);
	setFilter(filters, ui->chbFilterDstPort_2, ui->sbFilterDstPort_2, ui->leIp_2, ui->sbDstPort_2);
	setFilter(filters, ui->chbFilterDstPort_3, ui->sbFilterDstPort_3, ui->leIp_3, ui->sbDstPort_3);
	setFilter(filters, ui->chbFilterDstPort_4, ui->sbFilterDstPort_4, ui->leIp_4, ui->sbDstPort_4);
	setFilter(filters, ui->chbFilterDstPort_5, ui->sbFilterDstPort_5, ui->leIp_5, ui->sbDstPort_5);
	setFilter(filters, ui->chbFilterDstPort_6, ui->sbFilterDstPort_6, ui->leIp_6, ui->sbDstPort_6);
	setFilter(filters, ui->chbFilterDstPort_7, ui->sbFilterDstPort_7, ui->leIp_7, ui->sbDstPort_7);
	setFilter(filters, ui->chbFilterDstPort_8, ui->sbFilterDstPort_8, ui->leIp_8, ui->sbDstPort_8);
	setFilter(filters, ui->chbFilterDstPort_9, ui->sbFilterDstPort_9, ui->leIp_9, ui->sbDstPort_9);
	setFilter(filters, ui->chbFilterDstPort_10, ui->sbFilterDstPort_10, ui->leIp_10, ui->sbDstPort_10);

	mPCap->setFilter(filters);

    mModel.setHorizontalHeaderLabels(QStringList() << "num" << "id" << "size" << "data");
    //ui->lvOutput->setColumnWidth(2, 600);

	mPCap->start();
}

void MainWindow::onReceivePacketString(quint64 num, uint id, uint size, const QString &val)
{
    if(ui->chbShowPackets->isChecked()){
        mPackets.push_back(udpdata(num, id, size, val));
    }
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
    if(ui->chbShowPackets->isChecked()){
        while(!mPackets.empty() && id++ < 40){
            mModel.appendRow(mPackets.front().get());
            mPackets.pop_front();
        }
    }else{

    }

    if(mUseScroll){
        ui->lvOutput->scrollToBottom();
    }

    if(mPCap){
        ui->statusbar->showMessage("Packets left " +QString::number(mPCap->packetsCount()));
    }
}

void MainWindow::on_pbClear_clicked()
{
    mPackets.clear();
    mModel.clear();
}

void MainWindow::on_cbUseScrollDown_clicked(bool checked)
{
	mUseScroll = checked;
}

void getFilterFromSettings(QSettings& settings, const QString& num, QCheckBox* cb, QSpinBox *sb, QLineEdit* le, QSpinBox* sb2)
{
	settings.beginGroup(num);

	ushort val = settings.value("filter_port").toUInt();
	ushort val2 = settings.value("port").toUInt();
	QString str = settings.value("ip").toString();

	cb->setChecked(settings.value("use").toBool());
	if(val)
		sb->setValue(val);
	if(!str.isEmpty())
		le->setText(str);
	if(val2)
		sb2->setValue(val2);

	settings.endGroup();
}

void setFilterToSettings(QSettings& settings, const QString& num, QCheckBox* cb, QSpinBox *sb, QLineEdit* le, QSpinBox* sb2)
{
	settings.beginGroup(num);

	settings.setValue("use", cb->isChecked());
	settings.setValue("filter_port", sb->value());
	settings.setValue("ip", le->text());
	settings.setValue("port", sb2->value());

	settings.endGroup();
}

void MainWindow::loadSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "org");

	mFileName = settings.value("filename").toString();
	ui->lbSelectedFile->setText(mFileName);

	getFilterFromSettings(settings, "flt1", ui->chbFilterDstPort, ui->sbFilterDstPort, ui->leIp, ui->sbDstPort);
	getFilterFromSettings(settings, "flt2", ui->chbFilterDstPort_2, ui->sbFilterDstPort_2, ui->leIp_2, ui->sbDstPort_2);
	getFilterFromSettings(settings, "flt3", ui->chbFilterDstPort_3, ui->sbFilterDstPort_3, ui->leIp_3, ui->sbDstPort_3);
	getFilterFromSettings(settings, "flt4", ui->chbFilterDstPort_4, ui->sbFilterDstPort_4, ui->leIp_4, ui->sbDstPort_4);
	getFilterFromSettings(settings, "flt5", ui->chbFilterDstPort_5, ui->sbFilterDstPort_5, ui->leIp_5, ui->sbDstPort_5);
	getFilterFromSettings(settings, "flt6", ui->chbFilterDstPort_6, ui->sbFilterDstPort_6, ui->leIp_6, ui->sbDstPort_6);
	getFilterFromSettings(settings, "flt7", ui->chbFilterDstPort_7, ui->sbFilterDstPort_7, ui->leIp_7, ui->sbDstPort_7);
	getFilterFromSettings(settings, "flt8", ui->chbFilterDstPort_8, ui->sbFilterDstPort_8, ui->leIp_8, ui->sbDstPort_8);
	getFilterFromSettings(settings, "flt9", ui->chbFilterDstPort_9, ui->sbFilterDstPort_9, ui->leIp_9, ui->sbDstPort_9);
	getFilterFromSettings(settings, "flt10", ui->chbFilterDstPort_10, ui->sbFilterDstPort_10, ui->leIp_10, ui->sbDstPort_10);
}

void MainWindow::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "org");

	settings.setValue("filename", mFileName);

	setFilterToSettings(settings, "flt1", ui->chbFilterDstPort, ui->sbFilterDstPort, ui->leIp, ui->sbDstPort);
	setFilterToSettings(settings, "flt2", ui->chbFilterDstPort_2, ui->sbFilterDstPort_2, ui->leIp_2, ui->sbDstPort_2);
	setFilterToSettings(settings, "flt3", ui->chbFilterDstPort_3, ui->sbFilterDstPort_3, ui->leIp_3, ui->sbDstPort_3);
	setFilterToSettings(settings, "flt4", ui->chbFilterDstPort_4, ui->sbFilterDstPort_4, ui->leIp_4, ui->sbDstPort_4);
	setFilterToSettings(settings, "flt5", ui->chbFilterDstPort_5, ui->sbFilterDstPort_5, ui->leIp_5, ui->sbDstPort_5);
	setFilterToSettings(settings, "flt6", ui->chbFilterDstPort_6, ui->sbFilterDstPort_6, ui->leIp_6, ui->sbDstPort_6);
	setFilterToSettings(settings, "flt7", ui->chbFilterDstPort_7, ui->sbFilterDstPort_7, ui->leIp_7, ui->sbDstPort_7);
	setFilterToSettings(settings, "flt8", ui->chbFilterDstPort_8, ui->sbFilterDstPort_8, ui->leIp_8, ui->sbDstPort_8);
	setFilterToSettings(settings, "flt9", ui->chbFilterDstPort_9, ui->sbFilterDstPort_9, ui->leIp_9, ui->sbDstPort_9);
	setFilterToSettings(settings, "flt10", ui->chbFilterDstPort_10, ui->sbFilterDstPort_10, ui->leIp_10, ui->sbDstPort_10);

	settings.sync();
}

void MainWindow::on_pbPause_clicked()
{
	if(mPCap){
		mPCap->pause();
	}
}

void MainWindow::on_sbTimeout_valueChanged(int arg1)
{
    if(mPCap){
        mPCap->setTimeout(arg1);
    }
}
