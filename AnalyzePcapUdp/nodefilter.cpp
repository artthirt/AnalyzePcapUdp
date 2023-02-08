#include "nodefilter.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qspinbox.h"

class NetWidget: public QWidget{
public:
    QLineEdit* ip = nullptr;
    QSpinBox*  port = nullptr;
};

NodeFilterDestination::NodeFilterDestination()
    : NodeBaseFilter()
{
}

QString NodeFilterDestination::caption() const
{
    return QObject::tr("Address Destination Filter");
}

QString NodeFilterDestination::name() const
{
    return "Address Destination Filter";
}

QWidget *NodeFilterDestination::embeddedWidget()
{
    if(!mUi){
        NetWidget* w = new NetWidget();
        QGridLayout *g = new QGridLayout(w);
        QLabel *sip = new QLabel("Ip  ", w);
        QLabel *spr = new QLabel("Port", w);
        w->ip = new QLineEdit(w);
        w->ip->setPlaceholderText("IP or Empty if not used");
        w->ip->setText(mIpSource.toString());
        w->port = new QSpinBox(w);
        w->port->setMaximum(65535);
        w->port->setValue(mPortSource);
        g->addWidget(sip, 0, 0);
        g->addWidget(spr, 1, 0);
        g->addWidget(w->ip, 0, 1);
        g->addWidget(w->port, 1, 1);

        QObject::connect(w->ip, &QLineEdit::textChanged, this, [this](QString text){
            if(text.isEmpty()){
                mIpSource = QHostAddress();
            }else{
                mIpSource = QHostAddress(text);
            }
        });
        QObject::connect(w->port, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mPortSource = val;
        });

        mUi.reset(w);
    }
    return mUi.get();
}

void NodeFilterDestination::compute(const PacketData &data)
{
    bool condition = (data.dstip == mIpSource && data.dstport == mPortSource)
            || (mIpSource.isNull() && data.dstport == mPortSource);
    if(!condition){
        return;
    }
    send_next(data);
}

QJsonObject NodeFilterDestination::save() const
{
    QJsonObject o =QtNodes::NodeDelegateModel::save();
    o["ip"] = mIpSource.toString();
    o["port"] = mPortSource;
    return o;
}

void NodeFilterDestination::load(const QJsonObject &o)
{
    QtNodes::NodeDelegateModel::load(o);
    mIpSource = QHostAddress(o["ip"].toString());
    mPortSource = o["port"].toInt(2000);
    if(mUi){
        if(mUi->ip)    mUi->ip->setText(mIpSource.toString());
        if(mUi->port)  mUi->port->setValue(mPortSource);
    }
}

/////////////////////////////////////

QString NodeFilterSource::caption() const
{
    return QObject::tr("Address Source Filter");
}

QString NodeFilterSource::name() const
{
    return "Address Source Filter";
}

void NodeFilterSource::compute(const PacketData &data)
{
    bool condition = (data.srcip == mIpSource && data.srcport == mPortSource)
            || (mIpSource.isNull() && data.srcport == mPortSource);
    if(!condition){
        return;
    }
    send_next(data);
}
