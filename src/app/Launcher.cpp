#include "Launcher.h"
#include <QBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include "../common/Constants.h"
#include "../common/IModule.h"
#include "../core/ConfigLoader.h"
#include "../core/HistoryManager.h"
#include "../core/HotkeyManager.h"
#include "../modules/Calculator.h"
#include "../modules/EverythingSearch.h"
#include "../modules/LauncherCommands.h"
#include "../widgets/ResultItemDelegate.h"
#include "../widgets/ResultItemWidget.h"

Launcher::Launcher(QWidget* parent) : QMainWindow(parent)
{
    // Register hotkey.
    m_hotkeyManager = new HotkeyManager(MOD_ALT, VK_SPACE, 0, this);
    connect(m_hotkeyManager, &HotkeyManager::hotkeyPressed, this,
            [&](const long long id)
            {
                if (id == 0)
                    setWindowVisibility(!isWindowShown);
            });

    // Set window attributes.
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    SetForegroundWindow(reinterpret_cast<HWND>(winId()));

    setupUi();
    setupModules();

    // Read configuration.
    const QJsonDocument doc = ConfigLoader::loadConfig(this);
    const QJsonObject rootObject = doc.object();
    const QJsonObject modulesObject = rootObject["modules"].toObject();
    for (ModuleConfig& config : m_moduleConfigs)
    {
        const QJsonObject moduleObject = modulesObject[ConfigLoader::toCamelCase(config.name)].toObject();
        config.enabled = moduleObject["enabled"].toBool();
        config.global = moduleObject["global"].toBool();
        config.priority = moduleObject["priority"].toInt();
        config.prefix = moduleObject["prefix"].toString()[0];

        if (!config.enabled)
        {
            disconnect(config.module, &IModule::resultsReady, this, &Launcher::onResultsReady);
            m_moduleConfigs.removeOne(config);
        }
    }
    const QJsonObject historyObject = rootObject["history"].toObject();
    m_decay = historyObject["decay"].toDouble();
    m_minScore = historyObject["minScore"].toDouble();
    m_increment = historyObject["increment"].toDouble();
    m_historyScoreWeight = historyObject["historyScoreWeight"].toDouble();

    // Configure history.
    HistoryManager::initHistory(m_decay, m_minScore);
    HistoryManager::setHistoryScoreWeight(m_historyScoreWeight);
}

QJsonDocument Launcher::defaultConfig() const
{
    QJsonObject rootObject;
    QJsonObject modulesObject;
    for (const ModuleConfig& config : m_moduleConfigs)
    {
        QJsonObject moduleObject;
        moduleObject["enabled"] = config.enabled;
        moduleObject["global"] = config.global;
        moduleObject["priority"] = config.priority;
        moduleObject["prefix"] = QString(config.prefix);
        modulesObject[ConfigLoader::toCamelCase(config.name)] = moduleObject;
    }
    rootObject["modules"] = modulesObject;
    QJsonObject historyObject;
    historyObject["decay"] = m_decay;
    historyObject["minScore"] = m_minScore;
    historyObject["increment"] = m_increment;
    historyObject["historyScoreWeight"] = m_historyScoreWeight;
    rootObject["history"] = historyObject;
    return QJsonDocument(rootObject);
}

/**
 * Show or hide the main window.
 *
 * @param visibility The new window visibility (true to show; false to hide).
 */
void Launcher::setWindowVisibility(const bool& visibility)
{
    isWindowShown = visibility;
    if (!visibility)
    {
        m_resultsList->clear();
        m_searchEdit->clear();
        hide(); // TODO: Fix flash on startup.
    }
    else
    {
        show();
        SetForegroundWindow(reinterpret_cast<HWND>(winId()));
    }
}

/**
 * Setup main window user interface.
 */
void Launcher::setupUi()
{
    // Main layout.
    resize(WINDOW_MARGIN + WINDOW_WIDTH + WINDOW_MARGIN, WINDOW_MARGIN + PADDING + BUTTON_SIZE + PADDING + WINDOW_SPACING + RESULT_LIST_HEIGHT + WINDOW_MARGIN);
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);
    m_centralWidget->setLayout(m_mainLayout);

    // Search area.
    QFont iconFont;
    iconFont.setFamily("Material Symbols Rounded");
    iconFont.setPixelSize(ICON_SIZE);
    m_searchFrame = new QFrame(this);
    m_searchFrame->setFixedHeight(PADDING + BUTTON_SIZE + PADDING);
    m_searchFrame->setFixedWidth(WINDOW_WIDTH);
    m_searchFrame->setStyleSheet(
        QString("QFrame { background-color: palette(base); border: 1px solid palette(alternate-base); border-radius: %1px; }").arg(CORNER_RADIUS));
    m_searchLayout = new QHBoxLayout(m_searchFrame);
    m_searchLayout->setContentsMargins(PADDING, PADDING, PADDING, PADDING);
    m_searchLayout->setSpacing(PADDING);
    m_searchIcon = new QLabel(this);
    m_searchIcon->setText(QChar(0xe8b6)); // Search.
    m_searchIcon->setFont(iconFont);
    m_searchIcon->setFixedWidth(BUTTON_SIZE);
    m_searchIcon->setFixedHeight(BUTTON_SIZE);
    m_searchIcon->setStyleSheet("QLabel { border: 1px solid transparent; background: transparent; }");
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Start typing...");
    m_searchEdit->setFixedHeight(BUTTON_SIZE);
    m_searchEdit->setFocus();
    m_searchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_searchEdit->setStyleSheet(QString("QLineEdit { border: none; background: transparent; font-size: %1px; padding: 0px; }").arg(TITLE_FONT_SIZE));
    connect(m_searchEdit, &QLineEdit::textChanged, this, &Launcher::onInputTextChanged);
    m_searchLayout->addWidget(m_searchIcon);
    m_searchLayout->addWidget(m_searchEdit);

    // Results list.
    m_resultsList = new QListWidget(this);
    m_resultsList->setFixedWidth(WINDOW_WIDTH);
    m_resultsList->setFixedHeight(RESULT_LIST_HEIGHT);
    m_resultsList->setFocusPolicy(Qt::NoFocus);
    m_resultsList->setMouseTracking(true);
    m_resultsList->setStyleSheet(
        QString("QListWidget { background-color: palette(base); border: 1px solid palette(alternate-base); border-radius: %1px; }").arg(CORNER_RADIUS));

    // Set custom delegate for results list.
    m_resultItemDelegate = new ResultItemDelegate(m_resultsList, this);
    m_resultsList->setItemDelegate(m_resultItemDelegate);
    connect(m_resultItemDelegate, &ResultItemDelegate::hideWindow, this, [&] { setWindowVisibility(false); });

    // Install event filter to handle keyboard navigation.
    m_searchEdit->installEventFilter(this);
    m_resultsList->installEventFilter(this);

    m_mainLayout->addWidget(m_searchFrame);
    m_mainLayout->addWidget(m_resultsList);
}

/**
 * Setup modules and connect to slots.
 */
void Launcher::setupModules()
{
    m_moduleConfigs = {ModuleConfig(new LauncherCommands(this), true, true, 5, ':'), ModuleConfig(new EverythingSearch(this), true, true, 1, '@'),
                       ModuleConfig(new Calculator(this), true, true, 5, '=')};

    // Connect all modules to results ready signal.
    for (ModuleConfig& config : m_moduleConfigs)
    {
        config.name = config.module->name();
        config.iconGlyph = config.module->iconGlyph();
        connect(config.module, &IModule::resultsReady, this, &Launcher::onResultsReady);
    }
}

/**
 * Handle results ready signal from modules.
 *
 * @param results The list of results to be displayed.
 * @param module The module providing the results.
 */
void Launcher::onResultsReady(QVector<ResultItem>& results, const IModule* module)
{
    int priority = 0;
    for (const ModuleConfig& config : m_moduleConfigs)
        if (config.module == module)
            priority = config.priority;

    for (auto& item : results)
    {
        item.priority = priority;
        const auto listItem = new ResultItemWidget(m_resultsList);
        listItem->setData(Qt::UserRole, QVariant::fromValue(item));
        m_resultsList->addItem(listItem);
    }

    m_resultsList->sortItems(Qt::DescendingOrder);
    if (m_resultsList->count() > 0)
        m_resultsList->setCurrentRow(0);
}

/**
 * Handle change of search text.
 *
 * @param text The search text.
 */
void Launcher::onInputTextChanged(const QString& text)
{
    m_resultsList->clear();
    m_searchIcon->setText(QChar(0xe8b6)); // Search.

    if (!text.isEmpty())
    {
        const QChar prefix = text.at(0);

        for (const ModuleConfig& config : m_moduleConfigs)
        {
            if (config.prefix == prefix)
            {
                m_searchIcon->setText(config.iconGlyph);
                config.module->query(text.mid(1).trimmed());
                return;
            }
        }

        for (const ModuleConfig& config : m_moduleConfigs)
        {
            if (config.global)
            {
                config.module->query(text.trimmed());
            }
        }
    }
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
        const auto keyEvent = dynamic_cast<QKeyEvent*>(event);

        // Navigate between actions.
        if (keyEvent->key() == Qt::Key_Tab)
        {
            handleActionsNavigation(true);
            return true;
        }
        if (keyEvent->key() == Qt::Key_Right)
        {
            if (m_searchEdit->cursorPosition() < m_searchEdit->text().length())
            {
                return QMainWindow::eventFilter(obj, event);
            }
            handleActionsNavigation(true);
            return true;
        }
        if (keyEvent->key() == Qt::Key_Left)
        {
            if (m_searchEdit->cursorPosition() > 0)
            {
                return QMainWindow::eventFilter(obj, event);
            }
            handleActionsNavigation(false);
            return true;
        }

        // Launch current action.
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            executeCurrentAction();
            return true;
        }

        // Navigate between results.
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
 *
 * @param right Whether to navigate right (true for right; false for left).
 */
void Launcher::handleActionsNavigation(const bool& right) const
{
    const QListWidgetItem* currentItem = m_resultsList->currentItem();
    if (!currentItem)
        return;

    const QVariant data = currentItem->data(Qt::UserRole);
    const auto item = data.value<ResultItem>();
    if (item.actions.isEmpty())
        return;

    // Get current action index from delegate.
    const int currentIndex = m_resultItemDelegate->getCurrentActionIndex();
    const int actionCount = static_cast<int>(item.actions.size());
    const int newIndex = (currentIndex + (right ? 1 : -1) + actionCount) % actionCount; // Add an additional actionCount to ensure newIndex > 0.

    m_resultItemDelegate->setCurrentActionIndex(newIndex);

    // Force repaint to show the highlight.
    m_resultsList->viewport()->update();
}

/**
 * Launch the highlighted action of the current result.
 */
void Launcher::executeCurrentAction()
{
    const QListWidgetItem* currentItem = m_resultsList->currentItem();
    if (!currentItem)
        return;

    const QVariant data = currentItem->data(Qt::UserRole);
    const auto item = data.value<ResultItem>();
    if (item.actions.isEmpty()) // Empty action list is accepted.
    {
        setWindowVisibility(false);
        return;
    }

    setWindowVisibility(false);

    // Execute the action at the current index.
    if (const int currentIndex = m_resultItemDelegate->getCurrentActionIndex(); currentIndex >= 0 && currentIndex < item.actions.size())
    {
        if (item.actions[currentIndex].handler)
            item.actions[currentIndex].handler();
        if (!item.key.isEmpty())
            HistoryManager::addHistory(item.key, m_increment);
    }
}
