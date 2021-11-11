#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QSettings>

#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    mPCap.reset(new PCapFile);

    mNetworker.reset(new Networker);

    connect(mPCap.data(), SIGNAL(sendPacketString(quint64, quint64, uint, uint, QString)),
            this, SLOT(onReceivePacketString(quint64, quint64, uint, uint, QString)), Qt::QueuedConnection);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    mTimer.start(16);

    mModel.setHorizontalHeaderLabels(QStringList() << "num" << "timestamp" << "id" << "size" << "data");

	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 3);

    ui->lvOutput->setModel(&mModel);

    ui->twWorkspace->setCurrentIndex(0);

    ui->lvOutput->setVisible(false);

    QList<int> szs = QList<int>() << 200 << 500;
    ui->splitterMain->setSizes(szs);

    initUiFilters();

	loadSettings();
}

MainWindow::~MainWindow()
{
	saveSettings();

    mNetworker.reset();

	delete ui;
}

void MainWindow::on_pbSelect_clicked()
{
    QFileInfo fi(mFileName);

    QString fn = QFileDialog::getOpenFileName(nullptr, "Open File...", fi.path(), "*.pcap;*.pcapng");

	if(fn.isEmpty())
		return;

	mFileName = fn;

	ui->lbSelectedFile->setText(fn);
}

void setFilter(QMap<ushort, Filter>& filters, QCheckBox* cb, QLineEdit* dstIp, QSpinBox* dstPort, QLineEdit* ip, QSpinBox* dstPort2)
{
	if(cb->isChecked()){
		Filter flt;
        flt.dstHost = QHostAddress(dstIp->text());
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

    updateTimeout();

    QMap<ushort, Filter> filters = getFilters();

	mPCap->setFilter(filters);

    mModel.setHorizontalHeaderLabels(QStringList() << "num" << "timestamp" << "id" << "size" << "data");
    //ui->lvOutput->setColumnWidth(2, 600);

	mPCap->start();
}

void MainWindow::onReceivePacketString(quint64 num, quint64 timestamp, uint id, uint size, const QString &val)
{
    if(ui->chbShowPackets->isChecked()){
        mPackets.push_back(udpdata(num, timestamp, id, size, val));
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
    ui->chbRepeat->setChecked(false);
    mPCap->stop();
    mPackets.clear();
}

QString getSize(qint64 size)
{
    QString res;

    if(size < 1e+3)
        return QString("%1 B").arg(size);
    if(size < 1e+6)
        return QString("%1 KB").arg(size / 1.e+3, 0, 'f', 2);
    if(size < 1e+9)
        return QString("%1 MB").arg(size / 1.e+6, 0, 'f', 2);
    return QString("%1 GB").arg(size / 1.e+9, 0, 'f', 2);

    return res;
}

void MainWindow::onTimeout()
{
    int id = 0;
    if(ui->chbShowPackets->isChecked()){
        while(!mPackets.empty() && id++ < 10000){
            mModel.appendRow(mPackets.front().get());
            mPackets.pop_front();
        }
        if(mUseScroll){
            ui->lvOutput->scrollToBottom();
        }
    }else{

    }

    if(mPCap){
        ui->statusbar->showMessage("Packets left " +QString::number(mPCap->packetsCount()));

        float pos = mPCap->position();

        ui->hsFilePosition->setValue(pos * ui->hsFilePosition->maximum());
    }

    QString out;
    for(int i = 0; i < mNetworker->sizeWorkers(); ++i){
        auto p = mNetworker->worker(i);
        out += QString("Destination Port:\t%1\n").arg(p->bindingPort());
        out += QString("  Received bytes:\t%1\n").arg(getSize(p->receivedSize()));
        out += QString("  Sended bytes:\t%1\n\n").arg(getSize(p->sendedSize()));
    }
    ui->lbNetOut->setText(out);
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

void getFilterFromSettings(QSettings& settings, const QString& num, QCheckBox* cb, QLineEdit* dstIp, QSpinBox *sb, QLineEdit* le, QSpinBox* sb2)
{
	settings.beginGroup(num);

	ushort val = settings.value("filter_port").toUInt();
	ushort val2 = settings.value("port").toUInt();
	QString str = settings.value("ip").toString();
    QString strD = settings.value("dst.ip").toString();

	cb->setChecked(settings.value("use").toBool());
	if(val)
		sb->setValue(val);
	if(!str.isEmpty())
		le->setText(str);
    if(!strD.isEmpty())
        dstIp->setText(strD);
    if(val2)
		sb2->setValue(val2);

	settings.endGroup();
}

void setFilterToSettings(QSettings& settings, const QString& num, QCheckBox* cb, QLineEdit* dstIp, QSpinBox *sb, QLineEdit* le, QSpinBox* sb2)
{
	settings.beginGroup(num);

	settings.setValue("use", cb->isChecked());
	settings.setValue("filter_port", sb->value());
	settings.setValue("ip", le->text());
    settings.setValue("dst.ip", dstIp->text());
    settings.setValue("port", sb2->value());

	settings.endGroup();
}

void MainWindow::loadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

	mFileName = settings.value("filename").toString();
    qint64 timeout = settings.value("timeout", 32).toInt();
    qint64 index = settings.value("index", 2).toInt();

	ui->lbSelectedFile->setText(mFileName);
    ui->cbSelectTimeout->setCurrentIndex(index);
    ui->sbTimeout->setValue(timeout);

    ui->twWorkspace->setCurrentIndex(settings.value("workspace", 0).toInt());

    updateTimeout();

    for(int i = 0; i < mUiFilters.size(); ++i){
        auto &It = mUiFilters[i];
        getFilterFromSettings(settings,  "flt" + QString::number(i), It.chk, It.dstIp, It.dstPort, It.outIp, It.outPort);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

	settings.setValue("filename", mFileName);
    settings.setValue("timeout", ui->sbTimeout->value());
    settings.setValue("index", ui->cbSelectTimeout->currentIndex());

    settings.setValue("workspace", ui->twWorkspace->currentIndex());

    for(int i = 0; i < mUiFilters.size(); ++i){
        auto &It = mUiFilters[i];
        setFilterToSettings(settings, "flt" + QString::number(i), It.chk, It.dstIp, It.dstPort, It.outIp, It.outPort);
    }

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
    updateTimeout();
}

void MainWindow::on_cbSelectTimeout_currentIndexChanged(int index)
{
    updateTimeout();
}

void MainWindow::updateTimeout()
{
    qint64 timeout = ui->sbTimeout->value();
    int index = ui->cbSelectTimeout->currentIndex();
    if(mPCap){
        mPCap->setTimeoutType(static_cast<PCapFile::TimeoutType>(index));
        mPCap->setTimeout(timeout);
    }
}


void MainWindow::on_pbNetStart_clicked()
{
    mNetworker->stopSending();

    auto filters = getFilters();
    for(auto &a: filters){
        mNetworker->addWorker(a.sndHost, a.sndPort, a.dstPort);
    }
}


void MainWindow::on_pbNetStop_clicked()
{
    mNetworker->stopSending();
}

QMap<ushort, Filter> MainWindow::getFilters()
{
    QMap<ushort, Filter> filters;
    for(int i = 0; i < mUiFilters.size(); ++i){
        auto &It = mUiFilters[i];
        setFilter(filters, It.chk, It.dstIp, It.dstPort, It.outIp, It.outPort);
    }
    return filters;
}


void MainWindow::on_pushButton_clicked()
{
    mNetworker->clearStatistic();
}


void MainWindow::on_chbRepeat_clicked(bool checked)
{
    if(mPCap){
        mPCap->setRepeat(checked);
    }
}

void MainWindow::initUiFilters()
{
    QGridLayout* gl = dynamic_cast<QGridLayout*>(ui->groupBoxFilters->layout());
    if(!gl)
        return;

    for(auto &it: mUiFilters){
        if(it.chk) delete it.chk;
        if(it.dstIp) delete it.dstIp;
        if(it.dstPort) delete it.dstPort;
        if(it.outIp) delete it.outIp;
        if(it.outPort) delete it.outPort;
    }
    mUiFilters.clear();

    ushort port = 10000;
    for(int i = 0; i < mUiFiltersCount; ++i){
        UiFilter ui;
        ui.chk = new QCheckBox(this);

        ui.dstIp = new QLineEdit(this);
        ui.dstIp->setInputMask("999.999.999.999");
        ui.dstIp->setText("");

        ui.dstPort = new QSpinBox(this);
        ui.dstPort->setMaximum(65535);
        ui.dstPort->setValue(port);

        ui.outIp = new QLineEdit(this);
        ui.outIp->setInputMask("999.999.999.999");
        ui.outIp->setText("127.0.0.1");

        ui.outPort = new QSpinBox(this);
        ui.outPort->setMaximum(65535);
        ui.outPort->setValue(port++);

        gl->addWidget(new QLabel(QString("%1. Use").arg(i + 1)), i, 0);
        gl->addWidget(ui.chk, i, 1);
        gl->addWidget(new QLabel("DestIp"), i, 2);
        gl->addWidget(ui.dstIp, i, 3);
        gl->addWidget(new QLabel("DestPort"), i, 4);
        gl->addWidget(ui.dstPort, i, 5);
        gl->addWidget(new QLabel("OutIp"), i, 6);
        gl->addWidget(ui.outIp, i, 7);
        gl->addWidget(new QLabel("OutPort"), i, 8);
        gl->addWidget(ui.outPort, i, 9);

        mUiFilters.push_back(ui);
    }
}

void MainWindow::on_pbHide_clicked()
{
    if(ui->lvOutput->isVisible()){
        ui->pbHide->setText("▼");
    }else{
        ui->pbHide->setText("▲");
    }
    ui->lvOutput->setVisible(!ui->lvOutput->isVisible());
}

