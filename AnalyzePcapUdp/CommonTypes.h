#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <QHostAddress>
#include <QString>
#include <QByteArray>

#include <functional>

struct PacketData{
    QHostAddress srcip;
    ushort srcport = 0;
    QHostAddress dstip;
    ushort dstport = 0;
    QByteArray data;

    PacketData(){}
    PacketData(const QByteArray& data, uint srcip, ushort srcport, uint dstip, ushort dstport){
        this->data = data;
        this->srcip = QHostAddress(srcip);
        this->dstip = QHostAddress(dstip);
        this->srcport = srcport;
        this->dstport = dstport;
    }
};
using PacketDataFun = std::function<void(const PacketData&)>;

#endif // COMMONTYPES_H
