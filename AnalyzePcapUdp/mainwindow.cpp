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

bool getFilterFromSettings(QSettings& settings, const QString& num, CfFilter& flt)
{
	settings.beginGroup(num);

    if(!settings.contains("use")){
        return false;
    }

    bool use = settings.value("use").toBool();

    QString dst_ip = settings.value("dst.ip").toString();
    ushort filter_port = settings.value("filter_port").toUInt();

    QString ip = settings.value("ip").toString();
    ushort port = settings.value("port").toUInt();

    flt.use = use;

    flt.dst_ip = dst_ip;
    flt.filter_port = filter_port;

    flt.ip = ip;
    flt.port = port;

	settings.endGroup();

    return true;
}

void setFilterToSettings(QSettings& settings, const QString& num, const CfFilter& flt)
{
    settings.beginGroup(num);

    settings.setValue("use", flt.use);

    settings.setValue("dst.ip", flt.dst_ip);
    settings.setValue("filter_port", flt.filter_port);

    settings.setValue("ip", flt.ip);
    settings.setValue("port", flt.port);

	settings.endGroup();
}

void MainWindow::loadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    mFileName       = settings.value("filename").toString();
    double timeout  = settings.value("timeout", 32).toDouble();
    qint64 index    = settings.value("index", 2).toInt();
    int nums        = settings.value("num_filters", 1).toInt();

	ui->lbSelectedFile->setText(mFileName);
    ui->dsbDelay->setValue(timeout);

    ui->twWorkspace->setCurrentIndex(settings.value("workspace", 0).toInt());

    updateTimeout();

    int i = 0;
    mFilters.clear();
    while(true){
        QString grp = "flt" + QString::number(i);
        CfFilter flt;
        if(!getFilterFromSettings(settings,  grp, flt)){
            break;
        }
        mFilters.push_back(flt);
        ++i;
    }
    ui->sbNumFilters->setValue(nums);
    initUiFilters(nums);
}

void MainWindow::saveSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

	settings.setValue("filename", mFileName);
    settings.setValue("timeout", ui->dsbDelay->value());
    settings.setValue("num_filters", mUiFilters.size());

    settings.setValue("workspace", ui->twWorkspace->currentIndex());

    int cnt = min(mUiFilters.size(), mFilters.size());
    for(int i = 0; i < cnt; ++i){
        auto &a1 = mUiFilters[i];
        auto &a2 = mFilters[i];

        a2.use = a1.chk->isChecked();
        a2.dst_ip = a1.dstIp->text();
        a2.filter_port = a1.dstPort->value();
        a2.ip = a1.outIp->text();
        a2.port = a1.outPort->value();
    }

    for(int i = 0; i < mFilters.size(); ++i){
        auto &It = mFilters[i];
        setFilterToSettings(settings, "flt" + QString::number(i), It);
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
    double timeout = ui->dsbDelay->value();
    if(mPCap){
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
        setFilter(filters, It.chk.get(), It.dstIp.get(), It.dstPort.get(), It.outIp.get(), It.outPort.get());
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

void MainWindow::initUiFilters(int nums)
{
    QGridLayout* gl = dynamic_cast<QGridLayout*>(ui->groupBoxFilters->layout());
    if(!gl)
        return;

    mUiFilters.clear();

    ushort port = 10000;
    mUiFilters.resize(nums);
    for(int i = 0; i < mUiFilters.size(); ++i){
        UiFilter &ui = mUiFilters[i];
        CfFilter flt;
        if(i < mFilters.size()){
            flt = mFilters[i];
        }

        ui.chk.reset(new QCheckBox());

        ui.dstIp.reset(new QLineEdit());
        //ui.dstIp->setInputMask("999.999.999.999");
        ui.dstIp->setText("");

        ui.dstPort.reset(new QSpinBox());
        ui.dstPort->setMaximum(65535);
        ui.dstPort->setValue(port);

        ui.outIp.reset(new QLineEdit());
        //ui.outIp->setInputMask("999.999.999.999");
        ui.outIp->setText("127.0.0.1");

        ui.outPort.reset(new QSpinBox());
        ui.outPort->setMaximum(65535);
        ui.outPort->setValue(port++);

        gl->addWidget(ui.newQLabel(QString("%1. Use").arg(i + 1)), i, 0);
        gl->addWidget(ui.chk.get(), i, 1);
        gl->addWidget(ui.newQLabel("DestIp(in packet)"), i, 2);
        gl->addWidget(ui.dstIp.get(), i, 3);
        gl->addWidget(ui.newQLabel("DestPort(in packet)"), i, 4);
        gl->addWidget(ui.dstPort.get(), i, 5);
        gl->addWidget(ui.newQLabel("| OutIp"), i, 6);
        gl->addWidget(ui.outIp.get(), i, 7);
        gl->addWidget(ui.newQLabel("OutPort"), i, 8);
        gl->addWidget(ui.outPort.get(), i, 9);

        ui.chk->setChecked(flt.use);
        ui.dstIp->setText(flt.dst_ip);
        ui.dstPort->setValue(flt.filter_port);
        ui.outIp->setText(flt.ip);
        ui.outPort->setValue(flt.port);
        //mUiFilters.push_back(ui);
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


void MainWindow::on_dsbDelay_valueChanged(double arg1)
{
    if(mPCap){
        mPCap->setTimeout(arg1);
    }
}

void MainWindow::on_sbNumFilters_valueChanged(int arg1)
{
}

void MainWindow::on_pbApply_clicked()
{
    initUiFilters(ui->sbNumFilters->value());
}

