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

    QTimer::singleShot(0, this, [this](){
        loadSettings();
    });
}

MainWindow::~MainWindow()
{
	saveSettings();

    delete ui;
}

void MainWindow::onUpdatePackets(const PacketData &data)
{

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


