#include "registrydatamodel.h"

#include <nodesource.h>
#include "nodefilter.h"
#include "nodeinfopackets.h"
#include "nodeudpsender.h"

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
    mRegistryModels.reset(new QtNodes::NodeDelegateModelRegistry());
    mRegistryModels->registerModel<NodeSource>("NodeSource");
    mRegistryModels->registerModel<NodeInfoPackets>("NodeSource");
    mRegistryModels->registerModel<NodeFilter>("NodeFilter");
    mRegistryModels->registerModel<NodeUdpSender>("NodeUdpSender");
}

