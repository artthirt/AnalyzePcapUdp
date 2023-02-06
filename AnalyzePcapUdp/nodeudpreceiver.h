#ifndef NODEUDPRECEIVER_H
#define NODEUDPRECEIVER_H

#include "CommonNodeTypes.h"

#include <QTimer>
#include <QElapsedTimer>

class QLabel;
class UdpThread;

class QSpinBox;
class QLineEdit;

class NodeUdpReceiver: public AncestorNode
{
public:
    NodeUdpReceiver();
    ~NodeUdpReceiver();

    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &);

    // NodeDelegateModel interface
public:
    QString caption() const;
    QString name() const;
    unsigned int nPorts(QtNodes::PortType portType) const;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex);
    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port);
    QWidget *embeddedWidget();

private:
    std::shared_ptr<PacketDataNode> mData;
    std::shared_ptr<QWidget> mUi;
    QLineEdit* mUiIp = nullptr;
    QSpinBox*  mUiPort = nullptr;

    std::shared_ptr<UdpThread> mUdp;

    QHostAddress mIp = QHostAddress::LocalHost;
    ushort mPort = 3000;

    QTimer mTimer;
    QElapsedTimer mElapsed;
    QLabel* mLb = nullptr;

    uint64_t mNumPacks = 0;
    uint64_t mCommonSize = 0;
    uint64_t mPickSize = 0;
    double mBitrate = 0;

    QString updateStats();

};

#endif // NODEUDPRECEIVER_H
