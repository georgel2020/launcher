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
class QPropertyAnimation;

class ResultItemDelegate;
class HotkeyManager;
class IModule;

class Launcher final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Launcher(QWidget *parent = nullptr);

    bool registerHotkey() const;

    [[nodiscard]] QJsonDocument defaultConfig() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onHotkeyPressed(long long id);
    void onInputTextChanged(const QString &text);
    void onResultsReady(QVector<ResultItem> &results, const IModule *module);

private:
    void setWindowVisibility(const bool &visibility);
    void setupUi();
    void configureModules();
    void readConfiguration();
    void handleActionsNavigation(const ResultItem& item, const bool &right) const;
    bool executeShortcutAction(const ResultItem& item, const QKeySequence &pressedShortcut);
    void executeCurrentAction(const ResultItem& item);

    bool isWindowShown = false;
    QWidget *m_centralWidget = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QFrame *m_searchFrame = nullptr;
    QHBoxLayout *m_searchLayout = nullptr;
    QLabel *m_searchIcon = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QListWidget *m_resultsList = nullptr;
    ResultItemDelegate *m_resultItemDelegate = nullptr;

    struct ModuleConfig
    {
        IModule *module;
        QString name;
        QChar iconGlyph;
        bool enabled;
        bool global;
        double priority;
        QChar prefix;

        ModuleConfig(IModule *module, const bool &enabled, const bool &global, const double &priority, const QChar &prefix)
        {
            this->module = module;
            this->name = QString();
            this->iconGlyph = QChar();
            this->enabled = enabled;
            this->global = global;
            this->priority = priority;
            this->prefix = prefix;
        }

        bool operator==(const ModuleConfig &other) const { return module == other.module; }
    };
    QVector<ModuleConfig> m_moduleConfigs;

    double m_historyDecay = 0.95;
    double m_historyMinScore = 0.01;
    double m_historyIncrement = 1.0;
    double m_historyScoreWeight = 1.0;

    int m_maxVisibleResults = 5;
    QString m_placeholderText = "Start typing...";
};
