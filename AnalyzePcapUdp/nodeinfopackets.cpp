#include "nodeinfopackets.h"

using namespace QtNodes;

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
    mData = Data;
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
        QObject::connect(mNameEdit.get(), &QLineEdit::textChanged, this, [this](const QString &arg){
           mName = arg;
        });
    }
    mNameEdit->setText(mName);
    return mNameEdit.get();
}
