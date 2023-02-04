#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <QHostAddress>
#include <QString>
#include <QByteArray>

#include <functional>

struct PacketData{
    uint64_t ID = 0;
    int64_t timestamp = 0;
    QHostAddress srcip;
    ushort srcport = 0;
    QHostAddress dstip;
    ushort dstport = 0;
    QByteArray data;

    PacketData(){}
    PacketData(uint64_t ID, int64_t timestamp,const QByteArray& data, uint srcip, ushort srcport, uint dstip, ushort dstport){
        this->ID = ID;
        this->timestamp = timestamp;
        this->data = data;
        this->srcip = QHostAddress(srcip);
        this->dstip = QHostAddress(dstip);
        this->srcport = srcport;
        this->dstport = dstport;
    }
    QString string() const{
        return QString("from %1:%2 to %3:%4")
                .arg(srcip.toString())
                .arg(srcport)
                .arg(dstip.toString())
                .arg(dstport);
    }
};
using PacketDataFun = std::function<void(const PacketData&)>;

#endif // COMMONTYPES_H
