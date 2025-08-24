#include "LauncherCommands.h"
#include <QApplication>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QTimer>
#include "../utils/ProcessUtils.h"

LauncherCommands::LauncherCommands(QObject *parent) : IModule(parent) {}

void LauncherCommands::query(const QString &text)
{
    QVector<ResultItem> results;

    if (QString("version").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "Version";
        item.subtitle = QString::fromStdString(__DATE__) + " " + QString::fromStdString(__TIME__);
        item.iconGlyph = QChar(0xe88e); // Info.
        item.key = "launcher_version";
        results.append(item);
    }
    if (QString("about").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "About Launcher";
        item.subtitle = "GitHub - georgel2020/launcher";
        item.iconGlyph = QChar(0xe88e); // Info.
        item.key = "launcher_about";
        Action aboutAction;
        aboutAction.handler = [] { QDesktopServices::openUrl(QUrl("https://github.com/georgel2020/launcher")); };
        item.actions = {aboutAction};
        results.append(item);
    }
    if (QString("exit").contains(text, Qt::CaseInsensitive) || QString("quit").contains(text, Qt::CaseInsensitive) ||
        QString("reload").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "Exit";
        item.subtitle = "Exit Launcher";
        item.iconGlyph = QChar(0xe879); // Exit to app.
        item.key = "launcher_exit";
        Action exitAction;
        exitAction.handler = [] { QApplication::quit(); };
        Action reloadAction;
        reloadAction.iconGlyph = QChar(0xe5d5); // Refresh.
        reloadAction.handler = []
        {
            ProcessUtils::startDetached(QApplication::arguments()[0], {}); // Does not work in Debug mode, when the build is a console application.
            QApplication::quit();
        };
        reloadAction.shortcut = QKeySequence(Qt::CTRL | Qt::Key_R);
        item.actions = {exitAction, reloadAction};
        results.append(item);
    }
    if (QString("configure").contains(text, Qt::CaseInsensitive))
    {
        ResultItem item;
        item.title = "Configure";
        item.subtitle = "Open Launcher configuration path";
        item.iconGlyph = QChar(0xe8b8); // Settings.
        item.key = "launcher_configure";
        Action configureAction;
        configureAction.handler = []
        {
            const QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).replace("/", "\\");
            ProcessUtils::startDetached("explorer", {configPath});
        };
        item.actions = {configureAction};
        results.append(item);
    }

    emit resultsReady(results, this);
}
