#ifndef NODEFILTER_H
#define NODEFILTER_H

#include "CommonNodeTypes.h"
#include "nodebasefilter.h"

#include <QHostAddress>

class NodeFilterDestination: public NodeBaseFilter
{
public:
    NodeFilterDestination();

    // NodeDelegateModel interface
public:
    QString caption() const;
    QString name() const;
    QWidget *embeddedWidget();

protected:
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
