#include <QApplication>
#include <QFontDatabase>
#include <QMessageBox>
#include "app/Launcher.h"
#include "utils/DialogUtils.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application information.
    QCoreApplication::setApplicationName("Launcher");

    // Load Font Awesome icon font.
    QFontDatabase::addApplicationFont(":/fonts/MaterialSymbolsRounded-Regular.ttf");

    // Create main window.
    Launcher launcher;

    if (!launcher.registerHotkey())
    {
        DialogUtils::showError("Failed to register hotkey. ");
        return 1;
    }

    return QApplication::exec();
}
