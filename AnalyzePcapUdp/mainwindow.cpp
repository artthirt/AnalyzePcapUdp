#include "mainwindow.h"
#include "CommonTypes.h"
#include "nodeinfopackets.h"
#include "nodesource.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QSettings>
#include <QScrollBar>
#include <QFontDatabase>
#include <QProcess>
#include <QMessageBox>

#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QJsonArray>

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

template<typename T>
QList<T *> getListNodes(DataFlowGraphModel* mModel)
{
    QList<T *> res;
    for(auto it: mModel->allNodeIds()){
        auto nodeType = mModel->nodeData(it, NodeRole::Type).toString();
        if(nodeType == T().name()){
            auto node = mModel->delegateModel<T>(it);
            if(node){
                res.push_back(node);
            }
        }
    }
    return res;
}

/////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    translator.load(":/translate_ru.qm");

    qRegisterMetaType<PacketData>("PacketData");

    auto registry = RegistryDataModel::getRegistryModel();
    mModel.reset(new QtNodes::DataFlowGraphModel(registry));
    mScene = new QtNodes::DataFlowGraphicsScene(*mModel);
    mView = new QtNodes::GraphicsView(mScene);
    ui->verticalLayout->addWidget(mView);

    mItemModels.setColumnCount(2);
    ui->lvListModels->setModel(&mItemModels);
    mItemModels.setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Path"));

    mInfoModel.setColumnCount(5);
    mInfoModel.setHorizontalHeaderLabels(QStringList() << QObject::tr("num")
                                         << QObject::tr("timestamp")
                                         << QObject::tr("id")
                                         << QObject::tr("size")
                                         << QObject::tr("data"));

    ui->lvOutput->setModel(&mInfoModel);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->peHex->setFont(fixedFont);

    connect(ui->lvOutput, &QTableView::clicked, this, &MainWindow::onInfoClicked);

    connect(mModel.get(), &DataFlowGraphModel::nodeCreated, this, [this](NodeId nodeId){
        auto val = mModel->nodeData(nodeId, NodeRole::Type);
        if(val.toString() == NodeInfoPackets().name()){
            auto node = mModel->delegateModel<NodeInfoPackets>(nodeId);
            updateInfoOutput();
            changeInfoOutput(node->id());
        }
    });

    connect(mModel.get(), &DataFlowGraphModel::nodeDeleted, this, [this](NodeId nodeId){
        auto node = mModel->delegateModel<NodeInfoPackets>(nodeId);
        if(node && mCurrentInfo == node){
            updateInfoOutput();
            changeInfoOutput(-1);
        }
    });

    connect(mScene, &DataFlowGraphicsScene::nodeSelected, this, [this](NodeId nodeId){
        auto type = mModel->nodeData(nodeId, NodeRole::Type).toString();
        if(type == NodeInfoPackets().name()){
            auto node = mModel->delegateModel<NodeInfoPackets>(nodeId);
            changeInfoOutput(node->id());
            for(int i = 0; i < ui->cbInfoList->count(); ++i){
                auto iid = ui->cbInfoList->itemData(i).toULongLong();
                if(iid == node->id()){
                    ui->cbInfoList->setCurrentIndex(i);
                    break;
                }
            }
        }
    });

    connect(ui->lvListModels, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        loadModel(index.row());
    });

    loadSettings();

    updateInfoOutput();

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

QString MainWindow::settingsFile()
{
    return "model.json";
}

std::vector< QByteArray > split(const QByteArray& in, int len)
{
    std::vector< QByteArray > res;
    res.resize(in.size() / len + 1);
    for(int i = 0; i < res.size(); ++i){
        int off1 = i * len;
        int off2 = i * len + len;
        const char *p1, *p2;
        if(off1 < in.size()){
            p1 = in.data() + off1;
            if(off2 > in.size())
                off2 = in.size() - off1;
            p2 = in.data() + off2;
            res[i] = QByteArray(p1, p2 - p1);
        }
    }

    return res;
}

void MainWindow::onInfoClicked(const QModelIndex &index)
{
    int row = index.row();
    auto it = mInfoModel.item(row);
    auto vba = it->data();
    if(vba.isNull()){
        return;
    }
    auto ba = vba.toByteArray();
    const int len = 16;
    auto sba = split(ba, len);
    QString res;
    int addr = 0;
    for(const auto &it: sba){
        if(it.isEmpty()){
            break;
        }
        res += QString("%1: ").arg(addr, 5, 16, QLatin1Char('0')) + it.toHex(' ');
        res += "\n";
        addr += len;
    }
    ui->peHex->setPlainText(res);
}

QList<QStandardItem*> getList(const PacketData &data)
{
    QList<QStandardItem*> ret;
    ret.push_back(new QStandardItem(QString::number(data.ID)));
    ret.back()->setData(data.data);
    QDateTime t = QDateTime::fromMSecsSinceEpoch(data.timestamp);
    ret.push_back(new QStandardItem(QString::number(1. * data.timestamp / 1e+3, 'f', 3)
                                    + QString(" (%1").arg(t.toString("yyyy-MM-dd hh:mm:ss:zzz"))));
    ret.push_back(new QStandardItem("ID " + QString::number(data.ID)));
    ret.push_back(new QStandardItem(QString::number(data.data.size())));
    ret.push_back(new QStandardItem(data.string()));
    return ret;
};

void MainWindow::onUpdatePackets(const PacketData &data)
{
    if(data.dstip.isNull()){
        mInfoModel.appendRow(new QStandardItem("-----"));
        return;
    }
    auto items = getList(data);
    mInfoModel.appendRow(items);
    if(ui->lvOutput->verticalScrollBar()->value() == ui->lvOutput->verticalScrollBar()->maximum()){
        ui->lvOutput->scrollToBottom();
    }
}

void MainWindow::updateInfoOutput()
{
    mInfoOutputs.clear();
    ui->cbInfoList->clear();
    for(auto it: mModel->allNodeIds()){
        auto node = mModel->delegateModel<AncestorNode>(it);
        if(node->name() == NodeInfoPackets().name()){
            auto nodeInfo = dynamic_cast<NodeInfoPackets*>(node);
            mInfoOutputs[node->id()] = nodeInfo;
            QString s = nodeInfo->nameEdit();
            ui->cbInfoList->addItem(nodeInfo->nameEdit(), nodeInfo->id());
            nodeInfo->disconnect(SIGNAL(nameEditChanged()));
            QObject::connect(nodeInfo, &NodeInfoPackets::nameEditChanged, this, [this](){
                updateInfoOutput();
            });
        }
    }
}

void MainWindow::changeInfoOutput(int64_t id)
{
    if(mCurrentInfo){
        mCurrentInfo->disconnect(SIGNAL(sendPacket(PacketData)));
    }

    if(mInfoOutputs.contains(id)){
        auto node = mInfoOutputs[id];
        if(node != mCurrentInfo){
            mInfoModel.clear();
        }
        mCurrentInfo = node;
        QObject::connect(mCurrentInfo, &NodeInfoPackets::sendPacket, this, &MainWindow::onUpdatePackets, Qt::QueuedConnection);
    }else{
        mCurrentInfo = nullptr;
    }
}

void MainWindow::loadSettings()
{
    auto model = loadJsonFromFile(settingsFile());
    if(!model.isEmpty()){
        mModel->load(model);
    }else{
        mModel->load(loadJsonFromString(defaultModel));
    }
    auto lang = model["language"].toString();
    if(lang.isEmpty() || lang == "en"){
        ui->actionEnglish->setChecked(true);
    }else if(lang == "ru"){
        ui->actionRussian->setChecked(true);
    }else{
    }

    auto lm = model["list_models"];
    mListModels.clear();
    if(lm.isArray()){
        auto arr = lm.toArray();
        for(int i = 0; i < arr.count(); ++i){
            auto val = arr[i];
            if(val.isString()){
                auto str = val.toString();
                if(QFile::exists(str)){
                    mListModels.push_back(val.toString());
                }
            }
        }
    }
    updateListModels();
}

void MainWindow::saveSettings()
{
    auto model = mModel->save();
    model["language"] = ui->actionRussian->isChecked()? "ru" : "en";

    if(!mListModels.empty()){
        QJsonArray arr;
        for(auto it: mListModels){
            arr.push_back(it);
        }
        model["list_models"] = arr;
    }

    saveJsonToFile(settingsFile(), model);
}

void MainWindow::updateListModels()
{
    mItemModels.clear();
    for(auto it: mListModels){
        QFileInfo fi(it);
        mItemModels.appendRow(QList<QStandardItem*>() << new  QStandardItem(fi.fileName()) << new QStandardItem(it));
    }
}

void MainWindow::clearView()
{
    mCurrentInfo = nullptr;
    mScene->clearScene();
    updateInfoOutput();
}

QList<NodeDelegateModel *> MainWindow::tmpListByName(const QString &name)
{
    QList<NodeDelegateModel *> res;
    for(auto it: mModel->allNodeIds()){
        auto nodeType = mModel->nodeData(it, NodeRole::Type).toString();
        if(nodeType == name){
            auto node = mModel->delegateModel<NodeDelegateModel>(it);
            res.push_back(node);
        }
    }
    return res;
}

void MainWindow::startAll()
{
    auto list = getListNodes<NodeSource>(mModel.get());
    if(!list.empty()){
        for(auto it: list){
            it->onStart();
        }
    }
}

void MainWindow::stopAll()
{
    auto list = getListNodes<NodeSource>(mModel.get());
    if(!list.empty()){
        for(auto it: list){
            it->onStop();
        }
    }
}

void MainWindow::pauseAll()
{
    auto list = getListNodes<NodeSource>(mModel.get());
    if(!list.empty()){
        for(auto it: list){
            it->onPause();
        }
    }
}

void MainWindow::on_pbClear_clicked()
{
    mInfoModel.clear();
}


void MainWindow::on_actionScale_Reset_triggered()
{
    mView->setupScale(1);
}

void MainWindow::on_actionScalePlus_triggered()
{
    mView->scaleUp();
}

void MainWindow::on_actionScaleMinus_triggered()
{
    mView->scaleDown();
}

void MainWindow::on_actionMove_To_Center_triggered()
{
    mView->centerScene();
}

void MainWindow::on_cbInfoList_currentIndexChanged(int index)
{
    auto id = ui->cbInfoList->itemData(index).toULongLong();
    changeInfoOutput(id);
}

void MainWindow::on_actionRussian_triggered(bool checked)
{
    if(QMessageBox::question(nullptr, tr("Change language"), tr("To change the language need restart the app. Do you want to continue?")) == QMessageBox::Yes){
        ui->actionEnglish->setChecked(false);
        ui->actionRussian->setChecked(true);
        saveSettings();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void MainWindow::on_actionEnglish_triggered(bool checked)
{
    if(QMessageBox::question(nullptr, tr("Change language"), tr("To change the language need restart the app. Do you want to continue?")) == QMessageBox::Yes){
        ui->actionEnglish->setChecked(true);
        ui->actionRussian->setChecked(false);
        saveSettings();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void MainWindow::on_actionStart_All_triggered()
{
    startAll();
}

void MainWindow::on_actionPause_All_triggered()
{
    pauseAll();
}

void MainWindow::on_actionStop_All_triggered()
{
    stopAll();
}

void MainWindow::on_pbClearView_clicked()
{
    if(QMessageBox::question(nullptr, tr("Clear View"), tr("This operation remove all nodes from screen. Do you want to continue?")) == QMessageBox::Yes){
        clearView();
    }
}

void MainWindow::on_pbLoadModel_clicked()
{
    auto model = ui->lvListModels->selectionModel();
    if(model){
        auto indexes = model->selection().indexes();
        for(auto it: indexes){
            loadModel(it.row());
            break;
        }
    }
}

void MainWindow::on_pbSaveCurrentModel_clicked()
{
    if(mModel->allNodeIds().empty()){
        qDebug("View is empty");
        QMessageBox::information(nullptr, tr("Warning"), tr("View is empty. Nothing to save."));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save model as..."), {}, "*.json");
    if(fileName.isEmpty())
        return;
    auto model = mModel->save();
    saveJsonToFile(fileName, model);
    if(!mListModels.contains(fileName)){
        mListModels.push_back(fileName);
    }
    updateListModels();
}

void MainWindow::loadModel(int id)
{
    if(id < 0 || id >= mListModels.size()){
        return;
    }

    if(QMessageBox::question(nullptr, tr("Clear View"), tr("This operation remove all nodes from screen. Do you want to continue?")) == QMessageBox::Yes){
    }else{
        return;
    }
    clearView();

    auto fn = mListModels[id];
    auto json = loadJsonFromFile(fn);
    mModel->load(json);

    updateInfoOutput();
}

void MainWindow::on_actionSave_Model_triggered()
{
    on_pbSaveCurrentModel_clicked();
}

