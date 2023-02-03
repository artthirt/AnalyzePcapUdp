#ifndef COMMONNODETYPES_H
#define COMMONNODETYPES_H

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

const QtNodes::NodeDataType ByteArrayType = {"ByteArray", "ByteArray"};

class ByteArrayData: public QtNodes::NodeData{
public:
   QByteArray Data;
    // NodeData interface
public:
    QtNodes::NodeDataType type() const {
        return ByteArrayType;
    }
};

#endif // COMMONNODETYPES_H
