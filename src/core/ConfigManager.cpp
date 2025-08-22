#include "ConfigManager.h"
#include <QDir>
#include <QStandardPaths>
#include "../app/Launcher.h"
#include "../common/IModule.h"

/**
 * Load configuration file for Launcher.
 *
 * If the file does not exist, a default configuration will be applied.
 *
 * @param launcher A pointer to Launcher main window.
 * @return The QJsonDocument representing the configuration.
 */
QJsonDocument ConfigManager::loadConfig(const Launcher *launcher) { return loadConfig(getConfigPath(), launcher->defaultConfig()); }

/**
 * Load configuration file for a Launcher module.
 *
 * If the file does not exist, a default configuration will be applied.
 *
 * @param module A pointer to the module.
 * @return The QJsonDocument representing the configuration.
 */
QJsonDocument ConfigManager::loadConfig(const IModule *module) { return loadConfig(getConfigPath(module->name()), module->defaultConfig()); }

/**
 * Load configuration file from a path.
 *
 * If the file does not exist, a default configuration will be applied.
 *
 * @param configPath The path to the configuration file.
 * @param defaultConfig The default QJsonDocument configuration.
 * @return The retrieved configuration.
 */
QJsonDocument ConfigManager::loadConfig(const QString &configPath, const QJsonDocument &defaultConfig)
{
    QFile file(configPath);

    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray data = file.readAll();
        file.close();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error != QJsonParseError::NoError)
            return {};

        return doc;
    }

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(defaultConfig.toJson(QJsonDocument::Indented));
    file.close();
    return defaultConfig;
}

/**
 * Convert a space-separated string to camel case.
 * @param text The original string.
 * @return The new string in camel case.
 */
QString ConfigManager::toCamelCase(const QString &text)
{
    const QStringList parts = text.split(' ', Qt::SkipEmptyParts);
    QString result = parts.at(0).toLower();
    for (int index = 1; index < parts.size(); ++index)
    {
        const QString &word = parts.at(index);
        result.append(word.at(0).toUpper() + word.mid(1).toLower());
    }
    return result;
}

/**
 * Get the configuration file path for Launcher.
 *
 * If the path does not exist, create the folders automatically.
 *
 * @return The file path string.
 */
QString ConfigManager::getConfigPath()
{
    const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        return {};

    const QDir dir(configDir);
    if (!dir.exists())
        if (!dir.mkpath("."))
            return {};
    return dir.filePath("Launcher.json");
}

/**
 * Get the configuration file path for a module.
 *
 * If the path does not exist, create the folders automatically.
 *
 * @param moduleName Name of the module.
 * @return The file path string.
 */
QString ConfigManager::getConfigPath(const QString &moduleName)
{
    const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        return {};

    const QDir dir(configDir);
    if (!dir.exists())
        if (!dir.mkpath("."))
            return {};
    if (!dir.exists("Modules"))
        if (!dir.mkdir("Modules"))
            return {};
    return dir.filePath("Modules/" + moduleName + ".json");
}
