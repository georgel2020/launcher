#include "AppsSearch.h"
#include <QDirIterator>
#include <QJsonArray>
#include <QProcess>
#include <QStandardPaths>
#include <shlobj.h>
#include <windows.h>
#include "../core/ConfigManager.h"

AppsSearch::AppsSearch(QObject *parent) : IModule(parent)
{
    const QJsonDocument doc = ConfigManager::loadModuleConfig(this);
    const QJsonObject rootObject = doc.object();
    const QJsonArray appsArray = rootObject["apps"].toArray();
    for (const QJsonValue app : appsArray)
    {
        const QJsonObject appObject = app.toObject();
        const QString name = appObject["name"].toString();
        const QString path = appObject["path"].toString();
        const QJsonArray keywordsArray = appObject["keywords"].toArray();
        QVector<QString> keywords;
        for (const QJsonValue keyword : keywordsArray)
            keywords.append(keyword.toString());
        m_apps.append({name, path, keywords});
    }
}

QJsonDocument AppsSearch::defaultConfig() const
{
    QJsonObject rootObject;
    QJsonArray appsArray;

    const QVector<QString> startMenuPaths = {QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation), // User start menu.
                                             R"(C:\ProgramData\Microsoft\Windows\Start Menu\Programs)"}; // System start menu.
    for (const QString &path : startMenuPaths)
    {
        QDirIterator it(path, QStringList() << "*.lnk", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            const QString &shortcutPath = it.next();
            QString targetPath;
            if (!getShortcutPath(shortcutPath, targetPath))
                continue;

            QJsonObject appObject;
            appObject["name"] = QFileInfo(shortcutPath).baseName();
            appObject["path"] = targetPath;
            QJsonArray keywordsArray;
            keywordsArray.append(QFileInfo(shortcutPath).baseName().toLower().replace(" ", ""));
            appObject["keywords"] = keywordsArray;
            appsArray.append(appObject);
        }
    }
    rootObject["apps"] = appsArray;
    return QJsonDocument(rootObject);
}

void AppsSearch::query(const QString &text)
{
    QVector<ResultItem> results;

    for (const AppInfo &app : m_apps)
    {
        bool isMatch = false;
        for (const QString &keyword : app.keywords)
            if (keyword.contains(text.toLower()))
                isMatch = true;

        if (isMatch)
        {
            ResultItem item;
            item.title = app.name;
            item.subtitle = app.path;
            item.iconPath = app.path;
            Action openAction;
            openAction.handler = [app] { QProcess::startDetached(app.path); };
            Action openAdminAction;
            openAdminAction.iconGlyph = QChar(0xe9e0); // Shield.
            openAdminAction.handler = [app] { executeAdmin(app.path, QStringList()); };
            item.actions = {openAction, openAdminAction};
            item.key = "app_" + app.path;
            results.append(item);
        }
    }

    emit resultsReady(results, this);
}

/**
 * Execute an application with administrator privileges.
 *
 * @param executablePath The path to the executable.
 * @param arguments The command line arguments.
 */
void AppsSearch::executeAdmin(const QString &executablePath, const QStringList &arguments)
{
    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.nShow = SW_SHOWNORMAL;
    sei.lpVerb = L"runas";
    sei.lpFile = executablePath.toStdWString().c_str();
    sei.lpParameters = arguments.join(' ').toStdWString().c_str();

    ShellExecuteEx(&sei);
}

/**
 * Resolve a Windows shortcut and retrieve the path to its target file.
 *
 * @param shortcutPath The file path to the shortcut (.lnk) being resolved.
 * @param targetPath Reference to a QString that will hold the resolved target
 * path of the shortcut if successful.
 * @return Return true if the shortcut was successfully resolved and the target
 * path was retrieved; false otherwise.
 */
bool AppsSearch::getShortcutPath(const QString &shortcutPath, QString &targetPath)
{
    HRESULT hr;
    IShellLinkW *pShellLink;
    IPersistFile *pPersistFile;

    // Initialize COM.
    hr = CoInitialize(nullptr);
    if (FAILED(hr))
    {
        return false;
    }

    // Create an IShellLink object.
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void **)&pShellLink);
    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    // Query for IPersistFile.
    hr = pShellLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile);
    if (FAILED(hr))
    {
        pShellLink->Release();
        CoUninitialize();
        return false;
    }

    // Load the shortcut file.
    hr = pPersistFile->Load(shortcutPath.toStdWString().c_str(), STGM_READ);
    if (FAILED(hr))
    {
        pPersistFile->Release();
        pShellLink->Release();
        CoUninitialize();
        return false;
    }

    // Resolve the shortcut.
    hr = pShellLink->Resolve(nullptr, SLR_NO_UI | SLR_NOUPDATE | SLR_NOSEARCH | SLR_NOTRACK);
    if (FAILED(hr))
    {
        pPersistFile->Release();
        pShellLink->Release();
        CoUninitialize();
        return false;
    }

    // Get the path to the link target.
    wchar_t szGotPath[MAX_PATH];
    hr = pShellLink->GetPath(szGotPath, MAX_PATH, nullptr, SLGP_UNCPRIORITY); // Or SLGP_SHORTPATH etc.
    if (SUCCEEDED(hr))
    {
        targetPath = QString::fromWCharArray(szGotPath);
    }

    // Release interfaces.
    pPersistFile->Release();
    pShellLink->Release();

    // Uninitialize COM.
    CoUninitialize();

    return true;
}
