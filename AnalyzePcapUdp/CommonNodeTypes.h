#ifndef COMMONNODETYPES_H
#define COMMONNODETYPES_H

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "CommonTypes.h"

const QtNodes::NodeDataType ByteArrayType = {"Packet", "Packet"};

class SignalData{
public:
    SignalData(){}
    SignalData(uint32_t id, PacketDataFun fun): _id(id), _Main(fun){}

    SignalData& operator += (const SignalData& sig){
        _Signals[sig._id] = sig._Main;
        return *this;
    }
    SignalData& operator -= (uint32_t id){
        release(id);
        return *this;
    }

    void operator()(const PacketData& data) const {
        for(auto it = _Signals.begin(); it != _Signals.end(); ++it){
            it->second(data);
        }
    }
    void release(uint32_t id){
        _Signals.erase(id);
    }
private:
    uint32_t _id = 0;
    PacketDataFun _Main;
    std::unordered_map<uint32_t, PacketDataFun> _Signals;
};

class PacketDataNode: public QtNodes::NodeData{
public:
    SignalData datafun;
    // NodeData interface
    void operator()(const PacketData& data)const{
        datafun(data);
    }

public:
    QtNodes::NodeDataType type() const {
        return ByteArrayType;
    }
};

class AncestorNode: public QtNodes::NodeDelegateModel{
public:
    AncestorNode();
    virtual ~AncestorNode(){}

    uint64_t id() const { return mId; }

protected:
    uint64_t mId = 0;
};

void saveJsonToFile(const QString& name, const QJsonObject& json);
QJsonObject loadJsonFromFile(const QString& name);
QJsonObject loadJsonFromString(const QString& name);

#endif // COMMONNODETYPES_H
