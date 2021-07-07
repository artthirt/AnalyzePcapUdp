#include "udpworker.h"

#include <QTimer>

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
