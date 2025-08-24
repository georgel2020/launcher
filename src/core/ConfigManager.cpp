#include "ConfigManager.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include "../app/Launcher.h"
#include "../common/IModule.h"
#include "../utils/DialogUtils.h"

/**
 * Load configuration file for a Launcher module.
 *
 * If the file does not exist, a default configuration will be applied.
 *
 * @param module A pointer to the module.
 * @return The QJsonDocument representing the configuration.
 */
QJsonDocument ConfigManager::loadConfig(const IModule *module)
{
    return loadConfig(getConfigPath(QString(R"(Modules\%1.json)").arg(module->name())), module->defaultConfig());
}

/**
 * Load configuration file from a path.
 *
 * If the file does not exist, a default configuration will be applied.
 *
 * @param fileName Configuration file name. If the file is under a subdirectory,
 * backslashes are accepted.
 * @param defaultConfig The default QJsonDocument configuration.
 * @return The retrieved configuration.
 */
QJsonDocument ConfigManager::loadConfig(const QString &fileName, const QJsonDocument &defaultConfig)
{
    const QString configPath = getConfigPath(fileName);
    QFile file(configPath);

    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray data = file.readAll();
        file.close();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error != QJsonParseError::NoError)
        {
            DialogUtils::showError(QString("Failed to parse configuration file %1. ").arg(configPath));
            return defaultConfig;
        }

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
 * Get the configuration file path.
 *
 * If the path does not exist, create the folders automatically.
 *
 * @param fileName Name of the configuration file.
 * @return The file path string.
 */
QString ConfigManager::getConfigPath(const QString &fileName)
{
    const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    const QDir dir(configDir);
    const QFileInfo fileInfo(dir.filePath(fileName));
    if (!fileInfo.absoluteDir().exists())
        if (!fileInfo.absoluteDir().mkpath("."))
            return {};
    return dir.filePath(fileName);
}
