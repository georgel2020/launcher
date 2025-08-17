
#include "ConfigLoader.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include "../modules/IModule.h"

/**
 * Load configuration file for a Launcher module.
 *
 * If the file does not exist, a default configuration will be applied.
 *
 * @param module A pointer to the module.
 * @return The QJsonDocument representing the configuration.
 */
QJsonDocument ConfigLoader::loadModuleConfig(const IModule* module)
{
    if (!module)
        return {};

    QFile file(getModuleConfigPath(module->name()));

    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray data = file.readAll();
        file.close();
        QJsonParseError error;
        const QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error != QJsonParseError::NoError)
            return {};

        return doc;
    }
    const QJsonDocument doc = module->defaultConfig();
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return doc;
}

/**
 * Get the configuration file path for a module.
 *
 * If the path does not exist, create the folders automatically.
 *
 * @param moduleName Name of the module.
 * @return The file path string.
 */
QString ConfigLoader::getModuleConfigPath(const QString& moduleName)
{
    const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        return {};

    const QDir dir(configDir);
    if (!dir.exists())
        dir.mkpath(".");
    if (!dir.exists("Modules"))
        dir.mkdir("Modules");

    return dir.filePath("Modules/" + moduleName + ".json");
}
