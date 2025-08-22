#include "EverythingSearch.h"
#include <QApplication>
#include <QClipboard>
#include <QProcess>
#include "../../third-party/everything-sdk/include/Everything.h"
#include "../core/ConfigManager.h"

EverythingSearch::EverythingSearch(QObject *parent) : IModule(parent)
{
    const QJsonDocument doc = ConfigManager::loadConfig(this);
    const QJsonObject rootObject = doc.object();
    m_maxResults = rootObject["maxResults"].toInt();
    m_runCountWeight = rootObject["runCountWeight"].toDouble();
}

QJsonDocument EverythingSearch::defaultConfig() const
{
    QJsonObject rootObject;
    rootObject["maxResults"] = m_maxResults;
    rootObject["runCountWeight"] = m_runCountWeight;
    return QJsonDocument(rootObject);
}

void EverythingSearch::query(const QString &text)
{
    QVector<ResultItem> results;

    Everything_SetSearchW(text.toStdWString().c_str());
    Everything_SetMax(m_maxResults);
    Everything_SetSort(EVERYTHING_SORT_RUN_COUNT_DESCENDING);
    Everything_SetRequestFlags(EVERYTHING_REQUEST_FILE_NAME | EVERYTHING_REQUEST_PATH | EVERYTHING_REQUEST_RUN_COUNT);
    Everything_QueryW(true);
    if (const DWORD lastError = Everything_GetLastError(); lastError == EVERYTHING_ERROR_IPC)
    {
        ResultItem item;
        item.title = "Everything is not running";
        item.subtitle = "Everything Search";
        item.iconGlyph = QChar(0xf8b6); // Error.
        results.append(item);
    }
    else if (lastError == EVERYTHING_OK)
    {
        const DWORD numResults = Everything_GetNumResults();
        for (DWORD resultIndex = 0; resultIndex < numResults; ++resultIndex)
        {
            const QString fileName = QString::fromWCharArray(Everything_GetResultFileNameW(resultIndex));
            const QString filePath = QString::fromWCharArray(Everything_GetResultPathW(resultIndex));
            const int runCount = static_cast<int>(Everything_GetResultRunCount(resultIndex));

            ResultItem item;
            item.title = fileName;
            item.subtitle = item.iconPath = filePath + "\\" + fileName;
            Action openAction;
            openAction.handler = [filePath, fileName]
            {
                QProcess::startDetached("explorer", {filePath + "\\" + fileName});
                Everything_IncRunCountFromFileNameW((filePath + "\\" + fileName).toStdWString().c_str());
            };
            Action openPathAction;
            openPathAction.iconGlyph = QChar(0xe2c8); // Folder open.
            openPathAction.handler = [filePath, fileName]
            {
                QProcess::startDetached("explorer", {filePath});
                Everything_IncRunCountFromFileNameW((filePath + "\\" + fileName).toStdWString().c_str());
            };
            Action copyAction;
            copyAction.iconGlyph = QChar(0xe173); // File copy.
            copyAction.handler = [filePath, fileName] { QApplication::clipboard()->setText(filePath + "\\" + fileName); };
            Action copyPathAction;
            copyPathAction.iconGlyph = QChar(0xebbd); // Folder copy.
            copyPathAction.handler = [filePath] { QApplication::clipboard()->setText(filePath); };
            item.actions = {openAction, openPathAction, copyAction, copyPathAction};
            item.key = "everything_" + filePath + "\\" + fileName;
            item.score = 1 + log(runCount + 1) * m_runCountWeight;
            results.append(item);
        }
    }

    emit resultsReady(results, this);
}
