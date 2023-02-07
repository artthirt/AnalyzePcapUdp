#ifndef NODECUTTER_H
#define NODECUTTER_H

#include "nodebasefilter.h"

class CutterWidget;

class NodeCutter : public NodeBaseFilter
{
public:
    NodeCutter();

    // NodeDelegateModel interface
public:
    QString caption() const;
    QString name() const;
    QWidget *embeddedWidget();

    // NodeBaseFilter interface
protected:
    std::shared_ptr<CutterWidget> mUi;
    ushort mOff = 0;
    ushort mLen = 65535;

    void compute(const PacketData &data);

    // Serializable interface
public:
    QJsonObject save() const;
    void load(const QJsonObject &);
};

#endif // NODECUTTER_H
