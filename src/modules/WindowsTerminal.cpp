#include "WindowsTerminal.h"
#include <QFile>
#include <QJsonArray>
#include <QProcess>
#include <QStandardPaths>

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
            item.key = "terminal_" + profileName;
            Action openAction;
            openAction.handler = [profileName] { QProcess::startDetached("wt", {"-p", profileName}); };
            item.actions = {openAction};
            results.append(item);
        }
    }

    emit resultsReady(results, this);
}
