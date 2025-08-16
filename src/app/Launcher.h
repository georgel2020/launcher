#pragma once

#include <QBoxLayout>
#include <QFrame>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMainWindow>
#include <windows.h>
#include "../app/HotkeyManager.h"
#include "../common/Action.h"
#include "../common/ResultItem.h"
#include "../modules/IModule.h"
#include "../modules/LauncherCommands.h"
#include "../widgets/ResultItemDelegate.h"

class Launcher final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Launcher(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onInputTextChanged(const QString& text);
    void onResultsReady(const QVector<ResultItem>& results) const;

private:
    void showWindow(bool visibility);
    void setupUi();
    void setupModules();
    void handleTabNavigation() const;
    void executeCurrentAction();

    bool isWindowShown = false;
    QWidget* m_centralWidget = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;
    QFrame* m_searchFrame = nullptr;
    QHBoxLayout* m_searchLayout = nullptr;
    QLabel* m_searchIcon = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QListWidget* m_resultsList = nullptr;
    ResultItemDelegate* m_resultItemDelegate = nullptr;

    HotkeyManager* m_hotkeyManager;
    QList<IModule*> m_modules;
};
