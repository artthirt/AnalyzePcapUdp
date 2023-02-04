#ifndef NODEINFOPACKETS_H
#define NODEINFOPACKETS_H

#include <QWidget>
#include <QTimer>

#include "CommonNodeTypes.h"
#include "qlineedit.h"

class NodeInfoPackets: public AncestorNode
{
    Q_OBJECT
public:
    NodeInfoPackets();

    // NodeDelegateModel interface
public:
    QString caption() const;
    QString name() const;
    unsigned int nPorts(QtNodes::PortType portType) const;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex);
    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port);
    QWidget *embeddedWidget();

    QString nameEdit() const { return mName; }

signals:
    void sendPacket(const PacketData&);

private:
    QString mName;
    std::shared_ptr<QLineEdit> mNameEdit;
    std::shared_ptr<PacketDataNode> mData;

    std::list<PacketData> mPackets;


    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &o);
};

#endif // NODEINFOPACKETS_H
