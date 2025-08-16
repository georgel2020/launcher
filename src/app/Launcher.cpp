#include "Launcher.h"
#include <QApplication>
#include <QDebug>
#include <QListWidgetItem>
#include <QStyle>
#include <QVariant>
#include <QKeyEvent>
#include "../common/Action.h"
#include "../common/ResultItem.h"
#include "../widgets/ResultItemDelegate.h"

Launcher::Launcher(QWidget* parent)
{
    // Set window attributes.
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // Setup user interface.
    setupUi();

    show();
}

/**
 * Setup main window user interface.
 */
void Launcher::setupUi()
{
    // Main layout.
    resize(WINDOW_MARGIN + WINDOW_WIDTH + WINDOW_MARGIN, WINDOW_MARGIN + SEARCH_HEIGHT + WINDOW_SPACING + RESULT_LIST_HEIGHT + WINDOW_MARGIN);
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);
    m_centralWidget->setLayout(m_mainLayout);

    // Search area.
    QFont iconFont;
    iconFont.setFamily("Material Symbols Rounded");
    iconFont.setPixelSize(ICON_SIZE);
    m_searchFrame = new QFrame(this); // TODO: Improve search area UI.
    m_searchFrame->setFixedHeight(SEARCH_HEIGHT);
    m_searchLayout = new QHBoxLayout(m_searchFrame);
    m_searchIcon = new QLabel(this);
    m_searchIcon->setText(QChar(0xe8b6)); // Search.
    m_searchIcon->setFont(iconFont);
    m_searchIcon->setFixedWidth(ICON_SIZE);
    m_searchIcon->setFixedHeight(ICON_SIZE);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Start typing...");
    m_searchEdit->setFocus();
    m_searchLayout->addWidget(m_searchIcon);
    m_searchLayout->addWidget(m_searchEdit);

    // Results list.
    m_resultsList = new QListWidget(this);
    m_resultsList->setFixedHeight(RESULT_LIST_HEIGHT);

    // Set custom delegate for results list.
    m_resultItemDelegate = new ResultItemDelegate(this);
    m_resultsList->setItemDelegate(m_resultItemDelegate);

    // Install event filter to handle keyboard navigation.
    m_resultsList->installEventFilter(this);
    m_searchEdit->installEventFilter(this);

    m_mainLayout->addWidget(m_searchFrame);
    m_mainLayout->addWidget(m_resultsList);
}


/**
 * Filter and handle specific key press events for the application.
 *
 * @param obj The object that generated the event.
 * @param event The event to be filtered.
 * @return True if the event is handled, otherwise the default event filter behavior is applied.
 */
bool Launcher::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Right) // TODO: Improve arrow key behavior.
        {
            handleTabNavigation();
            return true;
        }
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            executeCurrentAction();
            return true;
        }
        if (keyEvent->key() == Qt::Key_Down)
        {
            if (m_resultsList->currentRow() + 1 < m_resultsList->count())
            {
                m_resultsList->setCurrentRow(m_resultsList->currentRow() + 1);
                m_resultItemDelegate->setCurrentActionIndex(0);
                return true;
            }
        }
        if (keyEvent->key() == Qt::Key_Up)
        {
            if (m_resultsList->currentRow() - 1 >= 0)
            {
                m_resultsList->setCurrentRow(m_resultsList->currentRow() - 1);
                m_resultItemDelegate->setCurrentActionIndex(0);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

/**
 * Handle tab navigation to go through actions of current result.
 */
void Launcher::handleTabNavigation() const
{
    const QListWidgetItem* currentItem = m_resultsList->currentItem();
    if (!currentItem) return;

    const QVariant data = currentItem->data(Qt::UserRole);
    const auto item = data.value<ResultItem>();
    if (item.actions.isEmpty()) return;

    // Get current action index from delegate.
    const int currentIndex = m_resultItemDelegate->getCurrentActionIndex();
    const int actionCount = static_cast<int>(item.actions.size());
    const int newIndex = (currentIndex + 1) % actionCount;

    m_resultItemDelegate->setCurrentActionIndex(newIndex);

    // Force repaint to show the highlight.
    m_resultsList->viewport()->update();
}

/**
 * Launch the highlighted action of the current result.
 */
void Launcher::executeCurrentAction() const
{
    const QListWidgetItem* currentItem = m_resultsList->currentItem();
    if (!currentItem) return;

    const QVariant data = currentItem->data(Qt::UserRole);
    const auto item = data.value<ResultItem>();
    if (item.actions.isEmpty()) return;

    // Get current action index from delegate.
    const int currentIndex = m_resultItemDelegate->getCurrentActionIndex();

    // Execute the action at the current index.
    if (currentIndex >= 0 && currentIndex < item.actions.size())
    {
        if (item.actions[currentIndex].handler)
        {
            item.actions[currentIndex].handler();
        }
    }
}
