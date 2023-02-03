#ifndef REGISTRYDATAMODEL_H
#define REGISTRYDATAMODEL_H

#include <QtNodes/NodeDelegateModelRegistry>

class RegistryDataModel{
public:
    static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> getRegistryModel();
private:
    static std::shared_ptr<RegistryDataModel> mInstance;

    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> mRegistryModels;

    RegistryDataModel();

};

#endif // REGISTRYDATAMODEL_H
