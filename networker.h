#ifndef NETWORKER_H
#define NETWORKER_H

#include <QThread>
#include <QUdpSocket>
#include <QVector>

#include "udpworker.h"

class Networker : public QThread
{
public:
    explicit Networker(QObject *parent = nullptr);
    ~Networker();

    void addWorker(const QHostAddress senderIp, ushort senderPort, ushort bindingPort);
    void setData(int index, const QHostAddress senderIp, ushort senderPort, ushort bindingPort);

    bool empty();
    size_t sizeWorkers() const { return mWorkers.size(); }
    PUdpWorker& worker(int index) { return mWorkers[index]; }
    void startStopSending(int index, bool start);
    bool isBinding(int id) const;
    bool isSending(int id) const;
    qint64 receivedSize(int id) const;
    qint64 sendedSize(int id) const;
    void clearStatistic(int id);

    void startSending(std::initializer_list<int> vals = std::initializer_list<int>());
    void startByDstPort(ushort dstPort);
    void stopSending();
    void clearStatistic();

protected:
    virtual void run();

private:
    QVector<PUdpWorker> mWorkers;
};

#endif // NETWORKER_H
