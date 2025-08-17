#include "LauncherCommands.h"

LauncherCommands::LauncherCommands(QObject* parent) : IModule(parent) {}

QString LauncherCommands::name() const { return "Launcher Commands"; }

void LauncherCommands::query(const QString& text)
{
    QVector<ResultItem> results;

    if (text == "version")
    {
        ResultItem item;
        item.title = "Version: ";
        item.subtitle = QString::fromStdString(__DATE__) + " " + QString::fromStdString(__TIME__);
        item.iconGlyph = QChar(0xe88e); // Info.
        results.append(item);
    }

    emit resultsReady(results);
}
