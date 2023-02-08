#ifndef NODECONDITION_H
#define NODECONDITION_H

#include "nodebasefilter.h"

class ConditionWidget;

class NodeCondition : public NodeBaseFilter
{
public:
    enum Condition{None, Equal, Less, Greater};
    enum Type{Len};

    NodeCondition();

    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &);
    QString caption() const;
    QString name() const;

    // NodeDelegateModel interface
public:
    unsigned int nPorts(QtNodes::PortType portType) const;
    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port);
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
    QWidget *embeddedWidget();

    // NodeBaseFilter interface
protected:
    std::shared_ptr<PacketDataNode> mResT;
    std::shared_ptr<PacketDataNode> mResF;
    std::shared_ptr<ConditionWidget> mUi;

    Condition mCondition = None;
    Type mType = Len;
    int mLength = 65535;

    void compute(const PacketData &data);
};

#endif // NODECONDITION_H
