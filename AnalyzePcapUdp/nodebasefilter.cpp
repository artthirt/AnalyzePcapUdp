#include "nodebasefilter.h"

NodeBaseFilter::NodeBaseFilter()
{

}

void NodeBaseFilter::compute(const PacketData &data)
{

}

QString NodeBaseFilter::caption() const
{
    return "Base Filter";
}

QString NodeBaseFilter::name() const
{
    return "Base Filter";
}

unsigned int NodeBaseFilter::nPorts(QtNodes::PortType portType) const
{
    return 1;
}

QtNodes::NodeDataType NodeBaseFilter::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeBaseFilter::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
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

std::shared_ptr<QtNodes::NodeData> NodeBaseFilter::outData(const QtNodes::PortIndex port)
{
    return mRes;
}

QWidget *NodeBaseFilter::embeddedWidget()
{
    return nullptr;
}
