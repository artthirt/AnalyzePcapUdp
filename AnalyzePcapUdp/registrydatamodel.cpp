#include "registrydatamodel.h"

#include <QtNodes/ConnectionStyle>

#include <nodesource.h>
#include "nodefilter.h"
#include "nodeinfopackets.h"
#include "nodeudpsender.h"
#include "nodeudpreceiver.h"
#include "nodecutter.h"
#include "nodecondition.h"

static void setStyle()
{
    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
  {
    "ConnectionStyle": {
      "UseDataDefinedColors": true
    }
  }
  )");
}

std::shared_ptr<RegistryDataModel> RegistryDataModel::mInstance;

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> RegistryDataModel::getRegistryModel()
{
    if(!mInstance){
        mInstance.reset(new RegistryDataModel);
    }
    return mInstance->mRegistryModels;
}

RegistryDataModel::RegistryDataModel()
{
    setStyle();

    mRegistryModels.reset(new QtNodes::NodeDelegateModelRegistry());
    mRegistryModels->registerModel<NodeSource>(QObject::tr("1. Input"));
    mRegistryModels->registerModel<NodeInfoPackets>(QObject::tr("2. Output"));
    mRegistryModels->registerModel<NodeFilterDestination>(QObject::tr("3. Filters"));
    mRegistryModels->registerModel<NodeFilterSource>(QObject::tr("3. Filters"));
    mRegistryModels->registerModel<NodeUdpSender>(QObject::tr("2. Output"));
    mRegistryModels->registerModel<NodeUdpReceiver>(QObject::tr("1. Input"));
    mRegistryModels->registerModel<NodeCutter>(QObject::tr("3. Filters"));
    mRegistryModels->registerModel<NodeCondition>(QObject::tr("3. Filters"));
}

