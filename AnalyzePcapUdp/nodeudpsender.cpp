#include "nodeudpsender.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qspinbox.h"

ushort PortSender = 2000;

NodeUdpSender::NodeUdpSender()
{
    mPort = PortSender++;
}

QString NodeUdpSender::caption() const
{
    return QObject::tr("UDP Sender");
}

QString NodeUdpSender::name() const
{
    return "UDP Sender";
}

unsigned int NodeUdpSender::nPorts(QtNodes::PortType portType) const
{
    if(portType == QtNodes::PortType::In){
        return 1;
    }
    return 0;
}

QtNodes::NodeDataType NodeUdpSender::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeUdpSender::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
    auto Data = std::dynamic_pointer_cast<PacketDataNode>(nodeData);
    if(mData){
        mData->datafun -= id();
    }
    mData = Data;
    mElapsed.restart();
    if(mData){
        mData->datafun += SignalData(id(), [this](const PacketData& data){
            if(!mIp.isNull()){
                mSock.writeDatagram(data.data, mIp, mPort);
                mNumPacks++;
                mPickSize += data.data.size();
                mCommonSize += data.data.size();
            }
        });
    }
}

std::shared_ptr<QtNodes::NodeData> NodeUdpSender::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QWidget *NodeUdpSender::embeddedWidget()
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

        mUiIp   = ip;
        mUiPort = pr;

        auto lb = new QLabel(updateStats(), w);
        mLb = lb;

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
        });
        QObject::connect(pr, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mPort = val;
        });

        mUi.reset(w);
    }
    return mUi.get();
}

QString NodeUdpSender::updateStats()
{
    if(mElapsed.elapsed() > 1000){
        mBitrate = 1. * mPickSize / mElapsed.elapsed() * 1000 * 8;
        mPickSize = 0;
        mElapsed.restart();
    }

    QString res;

    res += QString(" Packets Count %1\n").arg(mNumPacks);
    res += QString(" Bitrate       %1 Kb/s").arg(double(mBitrate / 1000.), 0, 'f', 3);

    return res;
}

QJsonObject NodeUdpSender::save() const
{
    auto o = QtNodes::NodeDelegateModel::save();
    o["ip"] = mIp.toString();
    o["port"] = mPort;
    return o;
}

void NodeUdpSender::load(const QJsonObject &o)
{
    QtNodes::NodeDelegateModel::load(o);
    mIp = QHostAddress(o["ip"].toString());
    mPort = o["port"].toInt(2000);

    if(mUiPort){
        mUiPort->setValue(mPort);
    }
    if(mUiIp){
        mUiIp->setText(mIp.toString());
    }
}
