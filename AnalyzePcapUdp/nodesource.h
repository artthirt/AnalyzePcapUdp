#ifndef NODESOURCE_H
#define NODESOURCE_H

#include <QWidget>
#include <QTimer>

#include "CommonNodeTypes.h"
#include "pcapfile.h"

class QLabel;

class NodeSource: public AncestorNode
{
    Q_OBJECT
public:
    NodeSource();
    ~NodeSource();

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
    std::shared_ptr<PCapFile> mPcap;
    std::shared_ptr<QWidget> mUi;
    QString mFileName;
    QTimer mTimer;
    double mTimeout = 32;
    QLabel *mOutLb = nullptr;

    void setFile(const QString& fileName);
    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &);
};





#endif // NODESOURCE_H
