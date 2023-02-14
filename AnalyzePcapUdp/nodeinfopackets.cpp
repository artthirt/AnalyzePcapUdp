#include "nodeinfopackets.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

using namespace QtNodes;

int NodeInfoCounter = 1;

class InfoWidget: public QWidget{
public:
    QLabel    *mLb = nullptr;
    QLineEdit* mEd = nullptr;

    InfoWidget(): QWidget(){
        QVBoxLayout* vl = new QVBoxLayout(this);

        mEd = new QLineEdit(this);
        mLb = new QLabel("\n", this);

        vl->addWidget(mEd);
        vl->addWidget(mLb);
        this->setLayout(vl);
    }
};

NodeInfoPackets::NodeInfoPackets()
{
}


QtNodes::NodeDataType NodeInfoPackets::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeInfoPackets::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
    auto Data = std::dynamic_pointer_cast<PacketDataNode>(nodeData);
    if(mData){
        mData->datafun -= id();
    }

    if(mNumPacks > 0){
        Q_EMIT(sendPacket(PacketData()));
    }

    mData = Data;
    mElapsed.restart();
    if(mData){
        mData->datafun += SignalData(id(), [this](const PacketData& data){
            Q_EMIT(sendPacket(data));
            mNumPacks++;
            mPickSize += data.data.size();
            mCommonSize += data.data.size();
        });
    }
}

std::shared_ptr<QtNodes::NodeData> NodeInfoPackets::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QString NodeInfoPackets::caption() const
{
    return tr("Info");
}

QString NodeInfoPackets::name() const
{
    return ("Info");
}

unsigned int NodeInfoPackets::nPorts(QtNodes::PortType portType) const
{
    if(portType == PortType::In){
        return 1;
    }
    return 0;
}

QWidget *NodeInfoPackets::embeddedWidget()
{
    if(!mUi){
        auto w = new InfoWidget();
        mUi.reset(w);

        w->mLb->setText(updateStats());

        if(mName.isEmpty())
            mName = tr("Output %1").arg(NodeInfoCounter++);
        QObject::connect(w->mEd, &QLineEdit::textChanged, this, [this](const QString &arg){
           mName = arg;
           Q_EMIT(nameEditChanged());
        });
        w->mEd->setText(mName);

        connect(&mTimer, &QTimer::timeout, this, [this](){
            if(mLb){
                mLb->setText(updateStats());
            }
        });
        mTimer.start(400);
    }
    return mUi.get();
}

QString NodeInfoPackets::updateStats()
{
    if(mElapsed.elapsed() > 1000){
        mBitrate = 1. * mPickSize / mElapsed.elapsed() * 1000 * 8;
        mPickSize = 0;
        mElapsed.restart();
    }

    QString res;

    res += tr(" Packets Count %1").arg(mNumPacks) + "\n";
    res += tr(" Bitrate       %1 Kb/s").arg(double(mBitrate / 1000.), 0, 'f', 3);

    return res;
}

QJsonObject NodeInfoPackets::save() const
{
    auto o = NodeDelegateModel::save();
    o["outname"] = mName;
    return o;
}

void NodeInfoPackets::load(const QJsonObject &o)
{
    NodeDelegateModel::load(o);
    mName = o["outname"].toString(mName);
    if(mUi){
        mUi->mEd->setText(mName);
    }
}
