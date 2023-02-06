#include "nodefilter.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qspinbox.h"

NodeFilterDestination::NodeFilterDestination()
{
    mRes.reset(new PacketDataNode);
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
        QWidget* w = new QWidget();
        QGridLayout *g = new QGridLayout(w);
        QLabel *sip = new QLabel("Ip  ", w);
        QLabel *spr = new QLabel("Port", w);
        QLineEdit* ip = new QLineEdit(w);
        ip->setPlaceholderText("IP or Empty if not used");
        ip->setText(mIpSource.toString());
        QSpinBox*  pr = new QSpinBox(w);
        pr->setMaximum(65535);
        pr->setValue(mPortSource);
        g->addWidget(sip, 0, 0);
        g->addWidget(spr, 1, 0);
        g->addWidget(ip, 0, 1);
        g->addWidget(pr, 1, 1);

        QObject::connect(ip, &QLineEdit::textChanged, this, [this](QString text){
            if(text.isEmpty()){
                mIpSource = QHostAddress();
            }else{
                mIpSource = QHostAddress(text);
            }
        });
        QObject::connect(pr, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
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
    (*mRes)(data);
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
    if(!mRes){
        mRes.reset(new PacketDataNode);
    }
    (*mRes)(data);
}
