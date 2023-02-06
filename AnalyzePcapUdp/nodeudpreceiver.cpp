#include "nodeudpreceiver.h"
#include <QUdpSocket>
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QThread>
#include <QGridLayout>

using namespace QtNodes;

ushort PortReceiver = 3000;

///////////////////////////////

class UdpThread: public QThread{
public:
    UdpThread(): QThread(){
        setObjectName("UdpReceiver");
        moveToThread(this);
        start();
    }
    ~UdpThread(){
        quit();
        wait();
    }

    void setAddress(const QHostAddress& ip, ushort port){
        mAddr = ip;
        mPort = port;
        QTimer::singleShot(0, this, [this](){
            bind();
        });
    }

    void addSignal(const SignalData& sig){
        mSignal += sig;
    }

    QHostAddress addr() const { return mAddr; }
    ushort port() const { return mPort; }

protected:
    virtual void run(){
        mSock.reset(new QUdpSocket);

        bind();

        QObject::connect(mSock.get(), &QUdpSocket::readyRead, this, [this](){
            while(mSock->hasPendingDatagrams()){
                mBuf.resize(mSock->pendingDatagramSize());
                mSock->readDatagram(mBuf.data(), mBuf.size(), &mSrcAddr, &mSrcPort);

                PacketData p;
                p.data = mBuf;
                p.ID = mID++;
                p.dstip = mAddr;
                p.srcip = mSrcAddr;
                p.dstport = mPort;
                p.srcport = mSrcPort;
                p.timestamp = QDateTime::currentMSecsSinceEpoch();

                mSignal(p);
            }
        });

        exec();

        mSock.reset();
    }

private:
    SignalData mSignal;
    std::shared_ptr<QUdpSocket> mSock;
    QHostAddress mAddr = QHostAddress::Any;
    QHostAddress mSrcAddr;
    ushort mSrcPort = 0;
    QByteArray mBuf;
    ushort mPort = PortReceiver++;
    uint64_t mID = 0;

    void bind(){
        if(mSock){
            mSock->abort();
            mSock->bind(mAddr, mPort);
        }
    }
};

///////////////////////////////

NodeUdpReceiver::NodeUdpReceiver()
{
    mData.reset(new PacketDataNode);
    mUdp.reset(new UdpThread);

    mUdp->addSignal(SignalData(id(), [this](const PacketData& data){
                mData->datafun(data);
                mNumPacks++;
                mPickSize += data.data.size();
                mCommonSize += data.data.size();
            }));
    mIp = mUdp->addr();
    mPort = mUdp->port();
}

NodeUdpReceiver::~NodeUdpReceiver()
{
    mUdp.reset();
}

QJsonObject NodeUdpReceiver::save() const
{
    auto o = AncestorNode::save();
    o["ip"] = mIp.toString();
    o["port"] = mPort;
    return o;
}

void NodeUdpReceiver::load(const QJsonObject &o)
{
    AncestorNode::load(o);
    mIp = QHostAddress(o["ip"].toString());
    mPort = o["port"].toInt(2000);

    if(mUiPort){
        mUiPort->setValue(mPort);
    }
    if(mUiIp){
        mUiIp->setText(mIp.toString());
    }
}

QString NodeUdpReceiver::caption() const
{
    return QObject::tr("UDP Receiver");
}

QString NodeUdpReceiver::name() const
{
    return "UDP Receiver";
}

unsigned int NodeUdpReceiver::nPorts(QtNodes::PortType portType) const
{
    if(portType == PortType::In){
        return 0;
    }
    return 1;
}

QtNodes::NodeDataType NodeUdpReceiver::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeUdpReceiver::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
}

std::shared_ptr<QtNodes::NodeData> NodeUdpReceiver::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QWidget *NodeUdpReceiver::embeddedWidget()
{
    if(!mUi){
        QWidget* w = new QWidget();
        QGridLayout *g = new QGridLayout();
        QLabel *sip = new QLabel("Ip", w);
        QLabel *spr = new QLabel("Port", w);
        QLineEdit* ip = new QLineEdit(w);
        ip->setText(mIp.toString());
        QSpinBox*  pr = new QSpinBox(w);
        pr->setMaximum(65535);
        pr->setValue(mPort);
        g->addWidget(sip, 0, 0);
        g->addWidget(spr, 1, 0);
        g->addWidget(ip, 0, 1);
        g->addWidget(pr, 1, 1);

        auto lb = new QLabel(updateStats(), w);
        mLb = lb;

        mUiIp   = ip;
        mUiPort = pr;

        auto vl = new QVBoxLayout(w);
        vl->addLayout(g);
        vl->addWidget(lb);

        w->setLayout(vl);

        connect(&mTimer, &QTimer::timeout, this, [this](){
            if(mLb){
                mLb->setText(updateStats());
            }
        });
        mTimer.start(400);

        QObject::connect(ip, &QLineEdit::textChanged, this, [this](QString text){
           mIp = QHostAddress(text);
           mUdp->setAddress(mIp, mPort);
        });
        QObject::connect(pr, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mPort = val;
            mUdp->setAddress(mIp, mPort);
        });

        mUi.reset(w);
    }
    return mUi.get();
}

QString NodeUdpReceiver::updateStats()
{
    if(mElapsed.elapsed() > 1000){
        mBitrate = 1. * mPickSize / mElapsed.elapsed() * 1000 * 8;
        mPickSize = 0;
        mElapsed.restart();
    }

    QString res;

    res += QString(" Packets Count %1\n").arg(mNumPacks);
    if(mBitrate > 1000){
        res += QString(" Bitrate       %1 Kb/s").arg(double(mBitrate / 1000.), 0, 'f', 3);
    }else{
        res += QString(" Bitrate       %1 b/s").arg(double(mBitrate), 0, 'f', 3);
    }

    return res;
}
