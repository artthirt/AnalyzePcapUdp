#ifndef COMMONNODETYPES_H
#define COMMONNODETYPES_H

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "CommonTypes.h"

const QtNodes::NodeDataType ByteArrayType = {"Packet", "Packet"};

class PacketDataNode: public QtNodes::NodeData{
public:
    // NodeData interface
public:
    QtNodes::NodeDataType type() const {
        return ByteArrayType;
    }
};

void saveJsonToFile(const QString& name, const QJsonObject& json);
QJsonObject loadJsonFromFile(const QString& name);
QJsonObject loadJsonFromString(const QString& name);

#endif // COMMONNODETYPES_H
