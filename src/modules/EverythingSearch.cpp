#include "EverythingSearch.h"
#include <QApplication>
#include <QClipboard>
#include "../../third-party/everything-sdk/include/Everything.h"
#include "../core/ConfigManager.h"
#include "../utils/ProcessUtils.h"

EverythingSearch::EverythingSearch(QObject *parent) : IModule(parent)
{
    const QJsonDocument doc = ConfigManager::loadConfig(this);
    const QJsonObject rootObject = doc.object();
    m_maxResults = rootObject["maxResults"].toInt();
    m_runCountWeight = rootObject["runCountWeight"].toDouble();
}

QJsonDocument EverythingSearch::defaultConfig() const
{
    // clang-format off
    const QJsonObject rootObject{
        {"maxResults", m_maxResults},
        {"runCountWeight", m_runCountWeight}
    };
    // clang-format on

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
        item.iconType = IconType::Font;
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
            item.iconType = IconType::Thumbnail;
            Action openAction;
            openAction.description = "Open";
            openAction.handler = [filePath, fileName]
            {
                ProcessUtils::startDetached("explorer", {filePath + "\\" + fileName});
                Everything_IncRunCountFromFileNameW((filePath + "\\" + fileName).toStdWString().c_str());
            };
            Action openPathAction;
            openPathAction.description = "Open path";
            openPathAction.iconGlyph = QChar(0xe2c8); // Folder open.
            openPathAction.handler = [filePath, fileName]
            {
                ProcessUtils::startDetached("explorer", {filePath});
                Everything_IncRunCountFromFileNameW((filePath + "\\" + fileName).toStdWString().c_str());
            };
            openPathAction.shortcut = QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E);
            Action copyAction;
            copyAction.description = "Copy";
            copyAction.iconGlyph = QChar(0xe173); // File copy.
            copyAction.handler = [filePath, fileName] { QApplication::clipboard()->setText(filePath + "\\" + fileName); };
            copyAction.shortcut = QKeySequence(Qt::CTRL | Qt::Key_C);
            Action copyPathAction;
            copyPathAction.description = "Copy path";
            copyPathAction.iconGlyph = QChar(0xebbd); // Folder copy.
            copyPathAction.handler = [filePath] { QApplication::clipboard()->setText(filePath); };
            copyPathAction.shortcut = QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C);
            item.actions = {openAction, openPathAction, copyAction, copyPathAction};
            item.key = "everything_" + filePath + "\\" + fileName;
            item.score = 1 + log(runCount + 1) * m_runCountWeight;
            results.append(item);
        }
    }

    emit resultsReady(results, this);
}
