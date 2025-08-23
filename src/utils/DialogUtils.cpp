#include "DialogUtils.h"
#include <QMessageBox>

void DialogUtils::showError(const QString &message) { showDialog(QMessageBox::Critical, message); }

void DialogUtils::showWarning(const QString &message) { showDialog(QMessageBox::Warning, message); }

void DialogUtils::showDialog(const QMessageBox::Icon &icon, const QString &message)
{
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/icons/launcher.png"));
    msgBox.setWindowTitle("Launcher");
    msgBox.setIcon(icon);
    msgBox.setText(message);
    msgBox.setWindowModality(Qt::ApplicationModal);
    msgBox.exec();
}
