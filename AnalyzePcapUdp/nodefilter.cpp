#include "nodefilter.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qspinbox.h"

NodeFilterDestination::NodeFilterDestination()
{
    mRes.reset(new PacketDataNode);
}

QString NodeFilterDestination::caption() const
{
    return "Address Destination Filter";
}

QString NodeFilterDestination::name() const
{
    return "Address Destination Filter";
}

unsigned int NodeFilterDestination::nPorts(QtNodes::PortType portType) const
{
    return 1;
}

QtNodes::NodeDataType NodeFilterDestination::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeFilterDestination::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
    auto Data = std::dynamic_pointer_cast<PacketDataNode>(nodeData);
    if(mData){
        mData->datafun -= id();
    }
    mData = Data;
    if(mData){
        mData->datafun += SignalData(id(), [this](const PacketData& data){
            compute(data);
        });
    }
}

std::shared_ptr<QtNodes::NodeData> NodeFilterDestination::outData(const QtNodes::PortIndex port)
{
    return mRes;
}

QWidget *NodeFilterDestination::embeddedWidget()
{
    if(!mUi){
        QWidget* w = new QWidget();
        QGridLayout *g = new QGridLayout(w);
        QLabel *sip = new QLabel("Ip Source", w);
        QLabel *spr = new QLabel("Port Source", w);
        QLineEdit* ip = new QLineEdit(w);
        ip->setText(mIpSource.toString());
        QSpinBox*  pr = new QSpinBox(w);
        pr->setMaximum(65535);
        pr->setValue(mPortSource);
        g->addWidget(sip, 0, 0);
        g->addWidget(spr, 1, 0);
        g->addWidget(ip, 0, 1);
        g->addWidget(pr, 1, 1);

        QObject::connect(ip, &QLineEdit::textChanged, this, [this](QString text){
           mIpSource = QHostAddress(text);
        });
        QObject::connect(pr, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mPortSource = val;
        });

        mUi.reset(w);
    }
    return mUi.get();
}

void NodeFilterDestination::compute(const PacketData &data)
{
    bool condition = (data.dstip == mIpSource && data.dstport == mPortSource)
            || (mIpSource.isNull() && data.dstport == mPortSource);
    if(!condition){
        return;
    }
    (*mRes)(data);
}

QJsonObject NodeFilterDestination::save() const
{
    QJsonObject o =QtNodes::NodeDelegateModel::save();
    o["ip"] = mIpSource.toString();
    o["port"] = mPortSource;
    return o;
}

void NodeFilterDestination::load(const QJsonObject &o)
{
    QtNodes::NodeDelegateModel::load(o);
    mIpSource = QHostAddress(o["ip"].toString());
    mPortSource = o["port"].toInt(2000);
}

/////////////////////////////////////

QString NodeFilterSource::caption() const
{
    return "Address Source Filter";
}

QString NodeFilterSource::name() const
{
    return "Address Source Filter";
}

void NodeFilterSource::compute(const PacketData &data)
{
    bool condition = (data.srcip == mIpSource && data.srcport == mPortSource)
            || (mIpSource.isNull() && data.srcport == mPortSource);
    if(!condition){
        return;
    }
    if(!mRes){
        mRes.reset(new PacketDataNode);
    }
    (*mRes)(data);
}
