#ifndef NODEFILTER_H
#define NODEFILTER_H

#include "CommonNodeTypes.h"

#include <QHostAddress>

class NodeFilterDestination: public AncestorNode
{
public:
    NodeFilterDestination();

    // NodeDelegateModel interface
public:
    QString caption() const;
    QString name() const;
    unsigned int nPorts(QtNodes::PortType portType) const;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex);
    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port);
    QWidget *embeddedWidget();

protected:
    std::shared_ptr<PacketDataNode> mData;
    std::shared_ptr<PacketDataNode> mRes;
    std::shared_ptr<QWidget> mUi;
    QHostAddress mIpSource;
    ushort mPortSource = 2000;

    virtual void compute(const PacketData& data);

    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &);
};

class NodeFilterSource: public NodeFilterDestination{
    Q_OBJECT
public:
    QString caption() const;
    QString name() const;

protected:
    virtual void compute(const PacketData& data);

};

#endif // NODEFILTER_H
