#pragma once

class Launcher;
class IModule;
class QJsonDocument;
class QString;

class ConfigManager final
{
public:
    ConfigManager() = delete;

    static QJsonDocument loadConfig(const IModule *module);
    static QJsonDocument loadConfig(const QString &fileName, const QJsonDocument &defaultConfig);
    static QString toCamelCase(const QString &text);
    static QString getConfigPath(const QString &fileName);
};
