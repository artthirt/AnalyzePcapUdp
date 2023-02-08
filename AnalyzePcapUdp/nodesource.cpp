#include "nodesource.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLabel>

using namespace QtNodes;

class SourceWidget: public QWidget{
public:
    QLabel* lbOut = nullptr;
    QDoubleSpinBox* dsbTm = nullptr;
    QSlider *slider = nullptr;
};

NodeSource::NodeSource()
{
    mData.reset(new PacketDataNode);
}

NodeSource::~NodeSource()
{
    if(mPcap){
        mPcap->stop();
        mPcap.reset();
    }
}

QString NodeSource::caption() const
{
    return QObject::tr("Source File");
}

QString NodeSource::name() const
{
    return "Source File";
}

unsigned int NodeSource::nPorts(QtNodes::PortType portType) const
{
    if(portType == PortType::In){
        return 0;
    }
    return 1;
}

QtNodes::NodeDataType NodeSource::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeSource::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
}

std::shared_ptr<QtNodes::NodeData> NodeSource::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QWidget *NodeSource::embeddedWidget()
{
    if(mUi){
        return mUi.get();
    }

    SourceWidget* w = new SourceWidget();
    mUi.reset(w);

    auto vb = new QVBoxLayout(w);
    auto pbO = new QPushButton(tr("Open file"), w);
    w->lbOut = new QLabel(tr("Name\nSize"), w);
    auto hl = new QHBoxLayout(w);
    auto pbPlay  = new QPushButton("►", w);
    auto pbPause = new QPushButton("▌▌", w);
    auto pbStop  = new QPushButton("■", w);
    w->slider  = new QSlider(w);
    w->slider->setOrientation(Qt::Horizontal);

    auto hl2 = new QHBoxLayout(w);
    auto lbT = new QLabel(tr("Timeout (ms)"), w);
    w->dsbTm = new QDoubleSpinBox(w);
    w->dsbTm->setMaximum(9999999);
    w->dsbTm->setDecimals(3);
    w->dsbTm->setValue(mTimeout);

    hl2->addWidget(lbT);
    hl2->addWidget(w->dsbTm);

    hl->addWidget(pbPlay);
    hl->addWidget(pbPause);
    hl->addWidget(pbStop);

    vb->addLayout(hl2);
    vb->addWidget(pbO);
    vb->addWidget(w->lbOut);
    vb->addLayout(hl);
    vb->addWidget(w->slider);

    w->setLayout(vb);

    QObject::connect(w->dsbTm, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double arg){
        mTimeout = arg;
        if(mPcap){
            mPcap->setTimeout(arg);
        }
    });

    QObject::connect(pbO, &QPushButton::clicked, this, [this](){
        auto fn = QFileDialog::getOpenFileName(nullptr, tr("Open File"), "", "*.pcap *.pcapng");
        if(!fn.isEmpty()){
            setFile(fn);
        }
    });
    QObject::connect(pbPlay, &QPushButton::clicked, this, [this](){
        if(mPcap && mPcap->isOpen()){
            mPcap->setTimeout(mTimeout);
            mPcap->start();
            mTimer.start(100);
        }
    });
    QObject::connect(pbPause, &QPushButton::clicked, this, [this](){
        if(mPcap && mPcap->isOpen()){
            mPcap->pause();
            mTimer.stop();
        }
    });
    QObject::connect(pbStop, &QPushButton::clicked, this, [this](){
        if(mPcap && mPcap->isOpen()){
            mPcap->stop();
            mTimer.stop();
        }
    });
    mTimer.disconnect();
    QObject::connect(&mTimer, &QTimer::timeout, this, [this, w](){
        if(mPcap){
            float pos = mPcap->position() * 100;
            w->slider->setValue(pos);
        }
    }, Qt::QueuedConnection);

    return mUi.get();
}

void NodeSource::setFile(const QString &fn)
{
    mFileName = fn;
    mPcap.reset(new PCapFile());
    mPcap->openFile(fn);

    mPcap->setPacketDataFun([this](const PacketData& data){
        if(mData){
            (*mData)(data);
        }
    });

    QFileInfo fi(fn);
    if(mUi && mUi->lbOut){
        QFile f(fn);
        auto size = f.size();
        QString out;
        out     =  tr("Name:\t\"%1\"\n").arg(fi.fileName());
        if(size < 1024 * 1024){
            out += tr("Size:\t%2 KB").arg(size / 1024.);
        }else{
            out += tr("Size:\t%2 MB").arg(size / 1024. / 1024.);
        }
        mUi->lbOut->setText(out);
    }
}

QJsonObject NodeSource::save() const
{
    auto obj = QtNodes::NodeDelegateModel::save();
    obj["file"] = mFileName;
    obj["timeout"] = mTimeout;
    return obj;
}

void NodeSource::load(const QJsonObject &obj)
{
    QtNodes::NodeDelegateModel::load(obj);
    mTimeout = obj["timeout"].toInt(32);
    mFileName = obj["file"].toString();

    if(!mFileName.isEmpty()){
        setFile(mFileName);
    }
    if(mUi && mUi->dsbTm){
        mUi->dsbTm->setValue(mTimeout);
    }
}
