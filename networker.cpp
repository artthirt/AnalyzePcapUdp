#include "networker.h"

Networker::Networker(QObject *parent) : QThread(parent)
{

}

Networker::~Networker()
{
    quit();
    wait();
}

void Networker::addWorker(const QHostAddress senderIp, ushort senderPort, ushort bindingPort)
{
    for(auto a: mWorkers){
        if(a->bindingPort() == bindingPort){
            qDebug("binding port already in scope");
            PUdpWorker& wrk = a;
            wrk->setSenderIp(senderIp);
            wrk->setSenderPort(senderPort);
            wrk->setBindingPort(bindingPort);
            wrk->startSending();
            return;
        }
    }

    PUdpWorker wrk(new UdpWorker);
    wrk->setSenderIp(senderIp);
    wrk->setSenderPort(senderPort);
    wrk->setBindingPort(bindingPort);
    wrk->startSending();
    mWorkers.push_back(wrk);
}

void Networker::setData(int index, const QHostAddress senderIp, ushort senderPort, ushort bindingPort)
{
    if(index < mWorkers.size()){
        mWorkers.resize(index + 1);
        mWorkers[index].reset(new UdpWorker);
    }
    mWorkers[index]->setSenderIp(senderIp);
    mWorkers[index]->setSenderPort(senderPort);
    mWorkers[index]->setBindingPort(bindingPort);
}

bool Networker::empty()
{
    return mWorkers.empty();
}

void Networker::startStopSending(int index, bool start)
{
    if(index < mWorkers.size()){
        mWorkers[index]->setEnableSending(start);
    }
}

bool Networker::isBinding(int id) const
{
    return mWorkers[id]->isBinding();
}

bool Networker::isSending(int id) const
{
    return mWorkers[id]->isStarted();
}

qint64 Networker::receivedSize(int id) const
{
    return mWorkers[id]->receivedSize();
}

qint64 Networker::sendedSize(int id) const
{
    return mWorkers[id]->sendedSize();
}

void Networker::clearStatistic(int id)
{
    mWorkers[id]->clearStatistic();
}

void Networker::startSending(std::initializer_list<int> vals)
{
    if(vals.size()){
        for(auto id: vals){
            mWorkers[id]->startSending();
        }
    }else{
        for(auto a: mWorkers){
            a->startSending();
        }
    }
}

void Networker::startByDstPort(ushort dstPort)
{
    for(auto a: mWorkers){
        if(a->bindingPort() == dstPort){
            a->startSending();
        }
    }
}

void Networker::stopSending()
{
    for(auto a: mWorkers){
        a->stopSending();
    }
}

void Networker::clearStatistic()
{
    for(auto a: mWorkers){
        a->clearStatistic();
    }
}

void Networker::run()
{

}
