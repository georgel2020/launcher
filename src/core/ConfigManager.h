#pragma once

class Launcher;
class IModule;
class QJsonDocument;
class QString;

class ConfigManager final
{
public:
    ConfigManager() = delete;

    static QJsonDocument loadConfig(const Launcher *launcher);
    static QJsonDocument loadConfig(const IModule *module);
    static QJsonDocument loadConfig(const QString &configPath, const QJsonDocument &defaultConfig);
    static QString toCamelCase(const QString &text);

private:
    static QString getConfigPath();
    static QString getConfigPath(const QString &moduleName);
};
