#include "nodecutter.h"

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

class CutterWidget: public QWidget{
public:
    CutterWidget(): QWidget{}{
        QGridLayout *g = new QGridLayout(this);
        setLayout(g);

        g->addWidget(new QLabel(tr("Offset")), 0, 0);
        sbOff = new QSpinBox(this);
        sbOff->setMaximum(65535);
        g->addWidget(sbOff, 0, 1);

        g->addWidget(new QLabel(tr("Length")), 1, 0);
        sbLen = new QSpinBox(this);
        sbLen->setMaximum(65535);
        sbLen->setValue(65535);
        g->addWidget(sbLen, 1, 1);
    }

    QSpinBox *sbOff = nullptr;
    QSpinBox *sbLen = nullptr;
};

NodeCutter::NodeCutter()
{

}


QString NodeCutter::caption() const
{
    return tr("Cutter");
}

QString NodeCutter::name() const
{
    return tr("Cutter");
}

QWidget *NodeCutter::embeddedWidget()
{
    if(!mUi){
        mUi.reset(new CutterWidget);

        QObject::connect(mUi->sbOff, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mOff = val;
        });
        QObject::connect(mUi->sbLen, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mLen = val;
        });
    }
    return mUi.get();
}

void NodeCutter::compute(const PacketData &data)
{
    bool condition = mOff < data.data.size() && mLen > 0;
    if(!condition){
        return;
    }
    auto newData = data;
    newData.data = data.data.mid(mOff, mLen);

    (*mRes)(newData);
}

QJsonObject NodeCutter::save() const
{
    QJsonObject o =QtNodes::NodeDelegateModel::save();
    o["off"] = mOff;
    o["len"] = mLen;
    return o;
}

void NodeCutter::load(const QJsonObject &o)
{
    QtNodes::NodeDelegateModel::load(o);
    mOff = o["off"].toInt(mOff);
    mLen = o["len"].toInt(mLen);
    if(mUi){
        mUi->sbOff->setValue(mOff);
        mUi->sbLen->setValue(mLen);
    }
}
