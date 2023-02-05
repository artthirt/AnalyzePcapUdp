#ifndef NODEUDPSENDER_H
#define NODEUDPSENDER_H

#include "CommonNodeTypes.h"
#include "qelapsedtimer.h"
#include "qlabel.h"
#include "qtimer.h"

#include <QUdpSocket>

class NodeUdpSender: public AncestorNode
{
    Q_OBJECT
public:
    NodeUdpSender();

    QString caption() const;
    QString name() const;
    unsigned int nPorts(QtNodes::PortType portType) const;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex);
    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port);
    QWidget *embeddedWidget();

private:
    std::shared_ptr<QWidget> mUi;
    QUdpSocket mSock;
    QHostAddress mIp = QHostAddress::LocalHost;
    ushort mPort = 3000;
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
    void load(const QJsonObject &);
};

#endif // NODEUDPSENDER_H
