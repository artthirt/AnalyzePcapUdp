#include "mainwindow.h"
#include "CommonTypes.h"
#include "nodefilter.h"
#include "nodeinfopackets.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QSettings>

#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>

#include "registrydatamodel.h"
#include "CommonNodeTypes.h"

using namespace QtNodes;

const QString defaultModel = "{\"connections\":[{\"inPortIndex\":0,\"intNodeId\":2,\"outNodeId\":0,\"outPortIndex\":0},{\"inPortIndex\":0,\"intNodeId\":1,\"outNodeId\":0,\"outPortIndex\":0}],\"nodes\":[{\"id\":0,\"internal-data\":{\"file\":\"\",\"model-name\":\"Source File\",\"timeout\":32},\"position\":{\"x\":-504,\"y\":-126}},{\"id\":1,\"internal-data\":{\"model-name\":\"Info\"},\"position\":{\"x\":7,\"y\":50}},{\"id\":2,\"internal-data\":{\"ip\":\"127.0.0.1\",\"model-name\":\"UDP Sender\",\"port\":2001},\"position\":{\"x\":46,\"y\":-133}}]}";

/**
void MainWindow::onTimeout()
{
    int id = 0;
    if(ui->chbShowPackets->isChecked()){
        while(!mPackets.empty() && id++ < 10000){
            mModel.appendRow(mPackets.front().get());
            mPackets.pop_front();
        }
        if(mUseScroll){
            ui->lvOutput->scrollToBottom();
        }
    }else{

    }

    if(mPCap){
        mLabStatus1->setText("Packets left " +QString::number(mPCap->packetsCount()));

        float pos = mPCap->position();

        mLabStatus3->setText(QString("Bitrate %1 Kb/s").arg(mPCap->bitrate() / 1000, 0, 'f', 2));

        ui->hsFilePosition->setValue(pos * ui->hsFilePosition->maximum());
    }

    QString out;
    for(int i = 0; i < mNetworker->sizeWorkers(); ++i){
        auto p = mNetworker->worker(i);
        out += QString("Destination Port:\t%1\n").arg(p->bindingPort());
        out += QString("  Received bytes:\t%1\n").arg(getSize(p->receivedSize()));
        out += QString("  Sended bytes:\t%1\n\n").arg(getSize(p->sendedSize()));
    }
    ui->lbNetOut->setText(out);
}
*/
/////////////////////////////////

QString getSize(qint64 size)
{
    QString res;

    if(size < 1e+3)
        return QString("%1 B").arg(size);
    if(size < 1e+6)
        return QString("%1 KB").arg(size / 1.e+3, 0, 'f', 2);
    if(size < 1e+9)
        return QString("%1 MB").arg(size / 1.e+6, 0, 'f', 2);
    return QString("%1 GB").arg(size / 1.e+9, 0, 'f', 2);

    return res;
}

/////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    qRegisterMetaType<PacketData>("PacketData");

    auto registry = RegistryDataModel::getRegistryModel();
    mModel.reset(new QtNodes::DataFlowGraphModel(registry));
    mScene = new QtNodes::DataFlowGraphicsScene(*mModel);
    mView = new QtNodes::GraphicsView(mScene);
    ui->verticalLayout->addWidget(mView);

    mInfoModel.setHorizontalHeaderLabels(QStringList() << "num" << "timestamp" << "id" << "size" << "data");

    ui->lvOutput->setModel(&mInfoModel);

    connect(mModel.get(), &DataFlowGraphModel::nodeCreated, this, [this](NodeId nodeId){
        auto val = mModel->nodeData(nodeId, NodeRole::Type);
        if(val.toString() == NodeInfoPackets().name()){

        }
    });

    connect(mModel.get(), &DataFlowGraphModel::nodeDeleted, this, [this](NodeId nodeId){
        auto node = mModel->delegateModel<NodeInfoPackets>(nodeId);
        if(mCurrentInfo == node){
            mCurrentInfo->disconnect(SIGNAL(sendPacket(PacketData)));
            mCurrentInfo = nullptr;
            mInfoModel.clear();
        }

    });

    connect(mScene, &DataFlowGraphicsScene::nodeSelected, this, [this](NodeId nodeId){
        auto type = mModel->nodeData(nodeId, NodeRole::Type).toString();
        if(type == NodeInfoPackets().name()){
            auto node = mModel->delegateModel<NodeInfoPackets>(nodeId);
            if(node == mCurrentInfo){
                mInfoModel.clear();
            }
            mCurrentInfo = node;
            QObject::connect(mCurrentInfo, &NodeInfoPackets::sendPacket, this, &MainWindow::onUpdatePackets);
        }
    });

    loadSettings();

    for(auto it: mModel->allNodeIds()){
        auto node = mModel->delegateModel<AncestorNode>(it);
        if(node){
            qDebug("node id: %d, type %s", node->id(), node->name().toUtf8().data());
        }
    }
}

MainWindow::~MainWindow()
{
	saveSettings();

    delete ui;
}

void MainWindow::onUpdatePackets(const PacketData &data)
{
    auto get = [](const PacketData &data){
        QList<QStandardItem*> ret;
        ret.push_back(new QStandardItem(QString::number(data.ID)));
        QDateTime t = QDateTime::fromMSecsSinceEpoch(data.timestamp);
        ret.push_back(new QStandardItem(QString::number(1. * data.timestamp / 1e+3, 'f', 3)
                                        + QString(" (%1").arg(t.toString("yyyy-MM-dd hh:mm:ss:zzz"))));
        ret.push_back(new QStandardItem("ID " + QString::number(data.ID)));
        ret.push_back(new QStandardItem(QString::number(data.data.size())));
        ret.push_back(new QStandardItem(data.string()));
        return ret;
    };
    auto items = get(data);
    mInfoModel.appendRow(items);

}

void MainWindow::loadSettings()
{
    auto model = loadJsonFromFile("model.json");
    if(!model.isEmpty()){
        mModel->load(model);
    }else{
        mModel->load(loadJsonFromString(defaultModel));
    }
}

void MainWindow::saveSettings()
{
    auto model = mModel->save();
    saveJsonToFile("model.json", model);
}


