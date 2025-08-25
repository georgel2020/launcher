#include "SystemCommands.h"
#include <windows.h>
#include "../utils/ProcessUtils.h"

SystemCommands::SystemCommands(QObject *parent) : IModule(parent) {}

void SystemCommands::query(const QString &text)
{
    QVector<ResultItem> results;

    if (QString("shutdown").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "Shutdown";
        item.subtitle = "Shutdown the system";
        item.iconGlyph = QChar(0xe8ac); // Power settings new.
        item.iconType = IconType::Font;
        item.key = "system_shutdown";
        Action shutdownAction;
        shutdownAction.handler = [] { ProcessUtils::startDetached("slidetoshutdown"); };
        item.actions = {shutdownAction};
        item.score = QString("shutdown").startsWith(text, Qt::CaseInsensitive) ? 2.0 : 1.0;
        results.append(item);
    }
    if (QString("restart").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "Restart";
        item.subtitle = "Restart the system";
        item.iconGlyph = QChar(0xe5d5); // Refresh.
        item.iconType = IconType::Font;
        item.key = "system_restart";
        Action restartAction;
        restartAction.handler = [] { ProcessUtils::startDetached("shutdown", {"-r", "-t", "0"}); };
        item.actions = {restartAction};
        item.score = QString("restart").startsWith(text, Qt::CaseInsensitive) ? 2.0 : 1.0;
        results.append(item);
    }
    if (QString("lock").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "Lock";
        item.subtitle = "Lock the system";
        item.iconGlyph = QChar(0xe897); // Lock.
        item.iconType = IconType::Font;
        item.key = "system_lock";
        Action lockAction;
        lockAction.handler = [] { LockWorkStation(); };
        item.actions = {lockAction};
        item.score = QString("lock").startsWith(text, Qt::CaseInsensitive) ? 2.0 : 1.0;
        results.append(item);
    }

    emit resultsReady(results, this);
}
