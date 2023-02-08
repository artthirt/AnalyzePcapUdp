#ifndef NODEBASEFILTER_H
#define NODEBASEFILTER_H

#include "CommonNodeTypes.h"

class NodeBaseFilter: public AncestorNode
{
public:
    NodeBaseFilter();

protected:
    std::shared_ptr<PacketDataNode> mData;
    std::shared_ptr<PacketDataNode> mRes;

    virtual void compute(const PacketData& data);
    virtual void send_next(const PacketData& data);

    // NodeDelegateModel interface
public:
    QString caption() const;
    QString name() const;
    unsigned int nPorts(QtNodes::PortType portType) const;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex);
    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port);
    QWidget *embeddedWidget();
};

#endif // NODEBASEFILTER_H
