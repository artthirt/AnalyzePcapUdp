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
    return "UDP Sender";
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
    auto Data = std::dynamic_pointer_cast<ByteArrayData>(nodeData);
    mData = Data;
}

std::shared_ptr<QtNodes::NodeData> NodeUdpSender::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QWidget *NodeUdpSender::embeddedWidget()
{
    if(!mUi){
        QWidget* w = new QWidget();
        QGridLayout *g = new QGridLayout(w);
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


QJsonObject NodeUdpSender::save() const
{
    QJsonObject o;
    o["ip"] = mIp.toString();
    o["port"] = mPort;
    return o;
}

void NodeUdpSender::load(const QJsonObject &o)
{
    mIp = QHostAddress(o["ip"].toString());
    mPort = o["port"].toInt(2000);
}
