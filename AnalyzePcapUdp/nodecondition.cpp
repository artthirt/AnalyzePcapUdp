#include "nodecondition.h"

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>

using namespace QtNodes;

class ConditionWidget: public QWidget{
public:
    QComboBox* cbCn = nullptr;
    QComboBox* cbTp = nullptr;
    QSpinBox*  sbLn = nullptr;

    ConditionWidget(): QWidget(){
        QGridLayout* g = new QGridLayout(this);
        setLayout(g);
        cbCn = new QComboBox(this);
        cbCn->addItem(tr("None"));
        cbCn->addItem(tr("Equal"));
        cbCn->addItem(tr("Less"));
        cbCn->addItem(tr("Greater"));
        cbTp = new QComboBox(this);
        cbTp->addItem(tr("Length"));
        sbLn = new QSpinBox(this);
        sbLn->setMaximum(999999999);
        sbLn->setValue(65535);
        g->addWidget(new QLabel(tr("Condition")), 0, 0);
        g->addWidget(new QLabel(tr("Type")), 1, 0);
        g->addWidget(new QLabel(tr("Length")), 2, 0);

        g->addWidget(cbCn, 0, 1);
        g->addWidget(cbTp, 1, 1);
        g->addWidget(sbLn, 2, 1);
    }
};

/////////////////////////

NodeCondition::NodeCondition()
{
    mResT.reset(new PacketDataNode);
    mResF.reset(new PacketDataNode);
}


QJsonObject NodeCondition::save() const
{
    auto o = NodeBaseFilter::save();
    o["condition"] = mCondition;
    o["type"] = mType;
    o["length"] = mLength;
    return o;
}

void NodeCondition::load(const QJsonObject &o)
{
    NodeBaseFilter::load(o);
    mCondition = static_cast<Condition>(o["condition"].toInt());
    mType = static_cast<Type>(o["type"].toInt());
    mLength = (o["length"].toInt());

    if(mUi){
        mUi->cbCn->setCurrentIndex(mCondition);
        mUi->cbTp->setCurrentIndex(mType);
        mUi->sbLn->setValue(mLength);
    }
}

unsigned int NodeCondition::nPorts(QtNodes::PortType portType) const
{
    if(portType == PortType::Out){
        return 2;
    }
    return 1;
}

std::shared_ptr<QtNodes::NodeData> NodeCondition::outData(const QtNodes::PortIndex port)
{
    if(port == 1){
        return mResT;
    }
    return mResF;
}

#define SEND_CONDITION(op)          \
if(data.data.size() op mLength){    \
    (*mResT)(data);                 \
}else{                              \
    (*mResF)(data);                 \
}

void NodeCondition::compute(const PacketData &data)
{
    if(mCondition == None){
        (*mResT)(data);
        return;
    }
    if(mType == Len){
        auto len = data.data.size();
        switch (mCondition) {
        case Equal:
            if(data.data.size() == mLength){
                (*mResT)(data);
            }else{
                (*mResF)(data);
            }
            break;
        case Less:
            if(data.data.size() < mLength){
                (*mResT)(data);
            }else{
                (*mResF)(data);
            }
            break;
        case Greater:
            if(data.data.size() > mLength){
                (*mResT)(data);
            }else{
                (*mResF)(data);
            }
            break;
        default:
            break;
        }
    }
}

NodeDataType NodeCondition::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    if(portType == PortType::In){
        return ByteArrayType;
    }
    auto dt = ByteArrayType;
    dt.name = "False";
    if(portIndex == 1){
        dt.name = "True";
    }
    return dt;
}


QWidget *NodeCondition::embeddedWidget()
{
    if(!mUi){
        mUi.reset(new ConditionWidget);
        QObject::connect(mUi->cbCn, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int val){
            mCondition = static_cast<Condition>(val);
        });
        QObject::connect(mUi->cbTp, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int val){
            mType = static_cast<Type>(val);
        });
        QObject::connect(mUi->sbLn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val){
            mLength = val;
        });

    }
    return mUi.get();
}


QString NodeCondition::caption() const
{
    return tr("Condition Filter");
}

QString NodeCondition::name() const
{
    return tr("Condition Filter");
}
