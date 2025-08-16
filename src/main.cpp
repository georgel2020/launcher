#include "app/Launcher.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // Load Font Awesome icon font.
    QFontDatabase::addApplicationFont(":/fonts/MaterialSymbolsRounded-Regular.ttf");

    // Create main window.
    Launcher w;
    return QApplication::exec();
}
