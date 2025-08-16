#pragma once

#include "../common/Constants.h"
#include "../modules/IModule.h"
#include "../widgets/ResultItemDelegate.h"
#include <QMainWindow>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>

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
    void setupUi();
    void setupModules();
    void handleTabNavigation() const;
    void executeCurrentAction() const;

    QWidget* m_centralWidget = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;
    QFrame* m_searchFrame = nullptr;
    QHBoxLayout* m_searchLayout = nullptr;
    QLabel* m_searchIcon = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QListWidget* m_resultsList = nullptr;
    ResultItemDelegate* m_resultItemDelegate = nullptr;

    QList<IModule*> m_modules;
};
