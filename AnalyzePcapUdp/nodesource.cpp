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

    QWidget* w = new QWidget();
    mUi.reset(w);

    auto vb = new QVBoxLayout(w);
    auto pbO = new QPushButton("Open file", w);
    auto lbO = new QLabel("", w);
    auto hl = new QHBoxLayout(w);
    auto pbPlay  = new QPushButton("►", w);
    auto pbPause = new QPushButton("▌▌", w);
    auto pbStop  = new QPushButton("■", w);
    auto slider  = new QSlider(w);
    slider->setOrientation(Qt::Horizontal);

    auto hl2 = new QHBoxLayout(w);
    auto lbT = new QLabel("Timeout (ms)", w);
    auto dsbT = new QDoubleSpinBox(w);
    dsbT->setMaximum(9999999);
    dsbT->setDecimals(3);
    dsbT->setValue(mTimeout);

    hl2->addWidget(lbT);
    hl2->addWidget(dsbT);

    hl->addWidget(pbPlay);
    hl->addWidget(pbPause);
    hl->addWidget(pbStop);

    vb->addLayout(hl2);
    vb->addWidget(pbO);
    vb->addWidget(lbO);
    vb->addLayout(hl);
    vb->addWidget(slider);

    w->setLayout(vb);

    mOutLb = lbO;

    QObject::connect(dsbT, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double arg){
        if(mPcap){
            mPcap->setTimeout(arg);
        }
    });

    QObject::connect(pbO, &QPushButton::clicked, this, [this](){
        auto fn = QFileDialog::getOpenFileName(nullptr, "Open File", "", "*.pcap *.pcapng");
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
    QObject::connect(&mTimer, &QTimer::timeout, this, [this, slider](){
        if(mPcap){
            float pos = mPcap->position() * 100;
            slider->setValue(pos);
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
    if(mOutLb){
        mOutLb->setText(fi.fileName());
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
}
