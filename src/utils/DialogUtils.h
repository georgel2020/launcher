#pragma once

#include <QObject>

class DialogUtils final
{
public:
    explicit DialogUtils() = delete;

    static void showError(const QString &message);
};
