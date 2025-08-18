#pragma once

#include <QMainWindow>
#include <windows.h>
#include "../common/Action.h"
#include "../common/ResultItem.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QFrame;
class QLineEdit;
class QListWidget;

class ResultItemDelegate;
class HotkeyManager;
class IModule;

class Launcher final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Launcher(QWidget* parent = nullptr);

    [[nodiscard]] QJsonDocument defaultConfig() const;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onInputTextChanged(const QString& text);
    void onResultsReady(QVector<ResultItem>& results, const IModule* module);

private:
    void setWindowVisibility(bool visibility);
    void setupUi();
    void setupModules();
    void handleActionsNavigation(bool right) const;
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

    struct ModuleConfig
    {
        IModule* module;
        QString name;
        QChar iconGlyph;
        bool enabled;
        bool global;
        int priority;
        QChar prefix;

        bool operator==(const ModuleConfig& other) const
        {
            return module == other.module;
        }
    };
    QVector<ModuleConfig> m_moduleConfigs;
};
