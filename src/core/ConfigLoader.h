#pragma once

#include <QObject>

class IModule;

class ConfigLoader final : public QObject
{
    Q_OBJECT

public:
    ConfigLoader() = delete;

    static QJsonDocument loadModuleConfig(const IModule* module);

private:
    static QString getModuleConfigPath(const QString& moduleName);
};
