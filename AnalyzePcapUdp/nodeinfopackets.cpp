#include "nodeinfopackets.h"

using namespace QtNodes;

int NodeInfoCounter = 1;

NodeInfoPackets::NodeInfoPackets()
{
}


QtNodes::NodeDataType NodeInfoPackets::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeInfoPackets::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
    auto Data = std::dynamic_pointer_cast<PacketDataNode>(nodeData);
    if(mData){
        mData->datafun -= id();
    }
    mData = Data;
    if(mData){
        mData->datafun += SignalData(id(), [this](const PacketData& data){
            Q_EMIT(sendPacket(data));
        });
    }
}

std::shared_ptr<QtNodes::NodeData> NodeInfoPackets::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QString NodeInfoPackets::caption() const
{
    return "Info";
}

QString NodeInfoPackets::name() const
{
    return "Info";
}

unsigned int NodeInfoPackets::nPorts(QtNodes::PortType portType) const
{
    if(portType == PortType::In){
        return 1;
    }
    return 0;
}

QWidget *NodeInfoPackets::embeddedWidget()
{
    if(!mNameEdit){
        mNameEdit.reset(new QLineEdit());
        if(mName.isEmpty())
            mName = QString("Output %1").arg(NodeInfoCounter++);
        QObject::connect(mNameEdit.get(), &QLineEdit::textChanged, this, [this](const QString &arg){
           mName = arg;
        });
    }
    mNameEdit->setText(mName);
    return mNameEdit.get();
}

QJsonObject NodeInfoPackets::save() const
{
    auto o = NodeDelegateModel::save();
    o["outname"] = mName;
    return o;
}

void NodeInfoPackets::load(const QJsonObject &o)
{
    NodeDelegateModel::load(o);
    mName = o["outname"].toString(mName);
}
