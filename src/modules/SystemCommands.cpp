#include "SystemCommands.h"
#include <QProcess>
#include <windows.h>

SystemCommands::SystemCommands(QObject* parent) : IModule(parent) {}

void SystemCommands::query(const QString& text)
{
    QVector<ResultItem> results;

    if (QString("shutdown").contains(text.toLower()) || QString("restart").contains(text.toLower()))
    {
        ResultItem item;
        item.title = "Shutdown";
        item.subtitle = "Shutdown the system";
        item.iconGlyph = QChar(0xe8ac); // Power settings new.
        item.key = "system_shutdown";
        Action shutdownAction;
        shutdownAction.handler = [] { QProcess::startDetached("slidetoshutdown"); };
        Action restartAction;
        restartAction.iconGlyph = QChar(0xe5d5); // Refresh.
        restartAction.handler = [] { QProcess::startDetached("shutdown", {"-r", "-t", "0"}); };
        item.actions = {shutdownAction, restartAction};
        results.append(item);
    }
    if (QString("lock").contains(text.toLower()))
    {
        ResultItem item;
        item.title = "Lock";
        item.subtitle = "Lock the system";
        item.iconGlyph = QChar(0xe897); // Lock.
        item.key = "system_lock";
        Action lockAction;
        lockAction.handler = [] { LockWorkStation(); };
        item.actions = {lockAction};
        results.append(item);
    }

    emit resultsReady(results, this);
}
