#pragma once

#include <QMessageBox>

class DialogUtils final
{
public:
    explicit DialogUtils() = delete;

    static void showError(const QString &message);
    static void showWarning(const QString &message);

private:
    static void showDialog(const QMessageBox::Icon &icon, const QString &message);
};
