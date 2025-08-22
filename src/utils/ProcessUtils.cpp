#include "ProcessUtils.h"
#include <QProcess>
#include <windows.h>

/**
 * Start a process with command line arguments.
 *
 * @param path The path to the executable.
 * @param arguments The command line arguments.
 * @param isAdmin Whether to start the process with administrator privileges.
 */
void ProcessUtils::startDetached(const QString &path, const QStringList &arguments, const bool &isAdmin)
{
    if (!isAdmin)
    {
        QProcess::startDetached(path, arguments);
    }
    else
    {
        SHELLEXECUTEINFO sei = {sizeof(sei)};
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.nShow = SW_SHOWNORMAL;
        sei.lpVerb = L"runas";
        sei.lpFile = path.toStdWString().c_str();
        sei.lpParameters = arguments.join(' ').toStdWString().c_str();

        ShellExecuteEx(&sei);
    }
}
