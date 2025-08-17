#include "EverythingSearch.h"
#include <QProcess>
#include "../core/ConfigLoader.h"

EverythingSearch::EverythingSearch(QObject* parent) : IModule(parent)
{
    const QJsonDocument doc = ConfigLoader::loadModuleConfig(this);
    const QJsonObject rootObject = doc.object();
    m_maxResults = rootObject["maxResults"].toInt();
}

QString EverythingSearch::name() const { return "Everything Search"; }

QJsonDocument EverythingSearch::defaultConfig() const
{
    QJsonObject rootObject;
    rootObject["maxResults"] = 50;
    return QJsonDocument(rootObject);
}

void EverythingSearch::query(const QString& text)
{
    QVector<ResultItem> results;

    Everything_SetSearchW(text.toStdWString().c_str());
    Everything_SetMax(m_maxResults);
    Everything_QueryW(true);
    if (const DWORD lastError = Everything_GetLastError(); lastError == EVERYTHING_ERROR_IPC)
    {
        results.append(ResultItem("Everything is not running. ", "Everything Search", QChar(0xf8b6), "", QVector<Action>()));
    }
    else if (lastError == EVERYTHING_OK)
    {
        const DWORD numResults = Everything_GetNumResults();
        for (DWORD resultIndex = 0; resultIndex < numResults; ++resultIndex)
        {
            const QString fileName = QString::fromWCharArray(Everything_GetResultFileNameW(resultIndex));
            const QString filePath = QString::fromWCharArray(Everything_GetResultPathW(resultIndex));

            ResultItem item;
            item.title = fileName;
            item.subtitle = item.iconPath = filePath + "\\" + fileName;
            Action openAction;
            openAction.handler = [fileName, filePath]() { QProcess::startDetached("explorer", {filePath + "\\" + fileName}); };
            Action openPathAction;
            openPathAction.iconGlyph = QChar(0xe2c8); // Folder open.
            openPathAction.handler = [filePath]() { QProcess::startDetached("explorer", {filePath}); };
            item.actions = {openAction, openPathAction};
            results.append(item);
        }
    }

    emit resultsReady(results);
}
