#ifndef NODEINFOPACKETS_H
#define NODEINFOPACKETS_H

#include <QWidget>
#include <QTimer>

#include "CommonNodeTypes.h"
#include <QTimer>
#include <QElapsedTimer>

class QLabel;

class InfoWidget;

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
    void nameEditChanged();

private:
    QString mName;
    std::shared_ptr<InfoWidget> mUi;
    std::shared_ptr<PacketDataNode> mData;
    QTimer mTimer;
    QElapsedTimer mElapsed;
    QLabel* mLb = nullptr;

    uint64_t mNumPacks = 0;
    uint64_t mCommonSize = 0;
    uint64_t mPickSize = 0;
    double mBitrate = 0;

    QString updateStats();

    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &o);
};

#endif // NODEINFOPACKETS_H
