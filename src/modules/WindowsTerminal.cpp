#include "WindowsTerminal.h"
#include <QFile>
#include <QJsonArray>
#include <QStandardPaths>
#include "../utils/ProcessUtils.h"

WindowsTerminal::WindowsTerminal(QObject *parent) : IModule(parent)
{
    const QString jsonPath =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + R"(\Packages\Microsoft.WindowsTerminal_8wekyb3d8bbwe\LocalState\settings.json)";
    if (QFile file(jsonPath); file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray data = file.readAll();
        file.close();
        QJsonParseError error;
        const QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error != QJsonParseError::NoError)
            return;

        const QJsonObject rootObject = doc.object();
        const QJsonArray profilesList = rootObject["profiles"].toObject()["list"].toArray();
        for (QJsonValue profile : profilesList)
        {
            const QString profileName = profile.toObject()["name"].toString();
            m_profileNames.append(profileName);
        }
    }
}

void WindowsTerminal::query(const QString &text)
{
    QVector<ResultItem> results;

    for (const QString &profileName : m_profileNames)
    {
        if (profileName.contains(text, Qt::CaseInsensitive))
        {
            ResultItem item;
            item.title = profileName;
            item.subtitle = "Open in Windows Terminal";
            item.iconGlyph = QChar(0xeb8e); // Terminal.
            item.iconType = IconType::Font;
            item.key = "terminal_" + profileName;
            Action openAction;
            openAction.handler = [profileName] { ProcessUtils::startDetached("wt", {"-p", profileName}); };
            Action openAdminAction;
            openAdminAction.iconGlyph = QChar(0xe9e0); // Shield.
            openAdminAction.handler = [profileName] { ProcessUtils::startDetached("wt", {"-p", profileName}, true); };
            openAdminAction.shortcut = QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Return);
            item.actions = {openAction, openAdminAction};
            item.score = profileName.startsWith(text, Qt::CaseInsensitive) ? 2.0 : 1.0;
            results.append(item);
        }
    }

    emit resultsReady(results, this);
}
