#pragma once

#include <QObject>

class Launcher;
class IModule;

class ConfigLoader final : public QObject
{
    Q_OBJECT

public:
    ConfigLoader() = delete;

    static QJsonDocument loadConfig(const Launcher* launcher);
    static QJsonDocument loadModuleConfig(const IModule* module);

private:
    static QString getConfigPath();
    static QString getModuleConfigPath(const QString& moduleName);
};
