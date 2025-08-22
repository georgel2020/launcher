#pragma once

#include <QObject>

class ProcessUtils final
{
public:
    ProcessUtils() = delete;

    static void startDetached(const QString &path, const QStringList &arguments = {}, const bool &isAdmin = false);
};
