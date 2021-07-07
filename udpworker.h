#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QThread>
#include <QUdpSocket>

#include <memory>

class UdpWorker : public QThread
{
public:
    explicit UdpWorker(QObject *parent = nullptr);
    ~UdpWorker();

    void setSenderIp(const QHostAddress& ip) { mSenderIp = ip; }
    QHostAddress senderIp() const { return mSenderIp; }

    void setSenderPort(ushort port) { mSenderPort = port; }
    ushort senderPort() const { return mSenderPort; }

    void setBindingPort(ushort port);
    ushort bindingPort() const { return mBindingPort; }

    qint64 receivedSize() const { return mReceivedSize; }
    qint64 sendedSize() const   { return mSendedSize; }

    QHostAddress receivedIp() const { return mReceivedHost; }
    ushort receivedPort() const { return mReceivedPort; }

    void clearStatistic();

    void startSending();
    void stopSending();
    void setEnableSending(bool enable);

    bool isStarted() const {
        return mStartSending;
    }
    bool isBinding() const{
        return mIsBinding;
    }

protected:
    virtual void run();

private:
    QScopedPointer<QUdpSocket> mSocket;
    QHostAddress mSenderIp;
    ushort mSenderPort = 1234;
    ushort mBindingPort = 1233;
    QByteArray mBuffer;
    bool mStartSending = false;
    bool mIsBinding = false;

    QHostAddress mReceivedHost;
    ushort mReceivedPort;

    qint64 mReceivedSize = 0;
    qint64 mSendedSize = 0;

    void binding();
    void readyRead();
};

typedef std::shared_ptr<UdpWorker> PUdpWorker;

#endif // UDPWORKER_H
