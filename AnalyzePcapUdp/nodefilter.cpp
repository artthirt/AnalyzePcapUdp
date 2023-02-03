#include "nodefilter.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qspinbox.h"

NodeFilter::NodeFilter()
{

}


QString NodeFilter::caption() const
{
    return "Source Filter";
}

QString NodeFilter::name() const
{
    return "Source Filter";
}

unsigned int NodeFilter::nPorts(QtNodes::PortType portType) const
{
    return 1;
}

QtNodes::NodeDataType NodeFilter::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    return ByteArrayType;
}

void NodeFilter::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex)
{
    auto Data = std::dynamic_pointer_cast<ByteArrayData>(nodeData);
    mData = Data;
}

std::shared_ptr<QtNodes::NodeData> NodeFilter::outData(const QtNodes::PortIndex port)
{
    return mData;
}

QWidget *NodeFilter::embeddedWidget()
{
    if(!mUi){
        QWidget* w = new QWidget();
        QGridLayout *g = new QGridLayout(w);
        QLabel *sip = new QLabel("Ip Source", w);
        QLabel *spr = new QLabel("Port Source", w);
        QLineEdit* ip = new QLineEdit(w);
        ip->setText(mIpSource.toString());
        QSpinBox*  pr = new QSpinBox(w);
        pr->setMaximum(65535);
        pr->setValue(mPortSource);
        g->addWidget(sip, 0, 0);
        g->addWidget(spr, 1, 0);
        g->addWidget(ip, 0, 1);
        g->addWidget(pr, 1, 1);

        QObject::connect(ip, &QLineEdit::textChanged, this, [this](QString text){
           mIpSource = QHostAddress(text);
        });
        QObject::connect(pr, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mPortSource = val;
        });

        mUi.reset(w);
    }
    return mUi.get();
}


QJsonObject NodeFilter::save() const
{
    QJsonObject o;
    o["ip"] = mIpSource.toString();
    o["port"] = mPortSource;
    return o;
}

void NodeFilter::load(const QJsonObject &o)
{
    mIpSource = QHostAddress(o["ip"].toString());
    mPortSource = o["port"].toInt(2000);
}
