#include "DialogUtils.h"
#include <QMessageBox>

void DialogUtils::showError(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/icons/launcher.png"));
    msgBox.setWindowTitle("Launcher");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(message);
    msgBox.setWindowModality(Qt::ApplicationModal);
    msgBox.exec();
}
