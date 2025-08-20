#pragma once

#include "../common/IModule.h"

class AppsSearch final : public IModule
{
    Q_OBJECT

public:
    explicit AppsSearch(QObject *parent = nullptr);

    [[nodiscard]] QString name() const override { return "Apps Search"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xe5c3); } // Apps.
    [[nodiscard]] QJsonDocument defaultConfig() const override;
    void query(const QString &text) override;

private:
    struct AppInfo
    {
        QString name;
        QString path;
        QVector<QString> keywords;
    };

    QVector<AppInfo> m_apps;

    static void executeAdmin(const QString &executablePath, const QStringList &arguments);
    static bool getShortcutPath(const QString &shortcutPath, QString &targetPath);
};
