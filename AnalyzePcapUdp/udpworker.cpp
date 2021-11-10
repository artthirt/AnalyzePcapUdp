#include "udpworker.h"

#include <QTimer>

#ifdef Q_OS_LINUX
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif

UdpWorker::UdpWorker(QObject *parent) : QThread(parent)
{
    moveToThread(this);
    start();
}

UdpWorker::~UdpWorker()
{
    quit();
    wait();
}

void UdpWorker::setBindingPort(ushort port)
{
    mBindingPort = port;

    QTimer::singleShot(0, this, [this](){
       binding();
    });
}

void UdpWorker::clearStatistic()
{
    mReceivedSize = 0;
    mSendedSize = 0;
}

void UdpWorker::startSending()
{
    mStartSending = true;
}

void UdpWorker::stopSending()
{
    mStartSending = false;
}

void UdpWorker::setEnableSending(bool enable)
{
    mStartSending = enable;
}

void UdpWorker::run()
{
    exec();
    mSocket.reset();
}

void UdpWorker::binding()
{
    if(mSocket){
        mSocket->abort();
        mIsBinding = mSocket->bind(mBindingPort);
    }else{
        mSocket.reset(new QUdpSocket);
        mSocket->moveToThread(this);
        mIsBinding = mSocket->bind(mBindingPort);
        connect(mSocket.data(), &QUdpSocket::readyRead, this, [this](){
            readyRead();
        });
    }

    const int SOCKET_BUFFER_SIZE = 1024 * 1024;

    int bufLen = SOCKET_BUFFER_SIZE;
    setsockopt(mSocket->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char*)&bufLen, sizeof(bufLen));
    setsockopt(mSocket->socketDescriptor(), SOL_SOCKET, SO_SNDBUF, (char*)&bufLen, sizeof(bufLen));
}

void UdpWorker::readyRead()
{
    while(mSocket->hasPendingDatagrams()){
        mBuffer.resize(mSocket->pendingDatagramSize());
        mReceivedSize += mBuffer.size();
        mSocket->readDatagram(mBuffer.data(), mBuffer.size(), &mReceivedHost, &mReceivedPort);
        if(mSenderPort && mStartSending){
            mSendedSize += mSocket->writeDatagram(mBuffer, mSenderIp, mSenderPort);
        }
    }
}
