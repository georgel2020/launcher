#pragma once

class Launcher;
class IModule;
class QJsonDocument;
class QString;

class ConfigLoader final
{
public:
    ConfigLoader() = delete;

    static QJsonDocument loadConfig(const Launcher *launcher);
    static QJsonDocument loadModuleConfig(const IModule *module);
    static QString toCamelCase(const QString &text);

private:
    static QString getConfigPath();
    static QString getModuleConfigPath(const QString &moduleName);
};
