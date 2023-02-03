#include "mainwindow.h"
#include "CommonTypes.h"
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

using namespace QtNodes;

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

    connect(mModel.get(), &QtNodes::DataFlowGraphModel::nodeCreated, this, [this](QtNodes::NodeId nodeId){
        auto val = mModel->nodeData(nodeId, NodeRole::Type);
        if(val.toString() == NodeInfoPackets().name()){

        }
    });

	loadSettings();
}

MainWindow::~MainWindow()
{
	saveSettings();

    delete ui;
}

void MainWindow::loadSettings()
{

}

void MainWindow::saveSettings()
{

}


