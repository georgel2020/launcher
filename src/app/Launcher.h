#pragma once

#include "../common/Constants.h"
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

private:
    void setupUi();

    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QFrame* m_searchFrame;
    QHBoxLayout* m_searchLayout;
    QLabel* m_searchIcon;
    QLineEdit* m_searchEdit;
    QListWidget* m_resultsList;
};
