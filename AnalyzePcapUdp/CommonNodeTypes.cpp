#include "CommonNodeTypes.h"

void saveJsonToFile(const QString &name, const QJsonObject &json)
{
    QFile f(name);
    if(!f.open(QIODevice::WriteOnly)){
        return;
    }
    f.write(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QJsonObject loadJsonFromFile(const QString &name)
{
    QFile f(name);
    if(!f.open(QIODevice::ReadOnly)){
        return {};
    }
    return QJsonDocument::fromJson(f.readAll()).object();
}

QJsonObject loadJsonFromString(const QString &name)
{
    return QJsonDocument::fromJson(name.toUtf8()).object();
}
