#include "Launcher.h"
#include <QApplication>
#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include "../common/Constants.h"
#include "../common/IModule.h"
#include "../core/ConfigManager.h"
#include "../core/HistoryManager.h"
#include "../core/HotkeyManager.h"
#include "../core/ThemeManager.h"
#include "../modules/AppsSearch.h"
#include "../modules/Calculator.h"
#include "../modules/EverythingSearch.h"
#include "../modules/LauncherCommands.h"
#include "../modules/SystemCommands.h"
#include "../modules/UnitConverter.h"
#include "../modules/WindowsTerminal.h"
#include "../widgets/ResultItemDelegate.h"
#include "../widgets/ResultItemWidget.h"

Launcher::Launcher(QWidget *parent) : QMainWindow(parent)
{
    // Set window attributes.
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    SetForegroundWindow(reinterpret_cast<HWND>(winId()));

    setupUi();
    setupModules();

    readConfiguration();

    // Configure history.
    HistoryManager::initHistory(m_decay, m_minScore, m_increment, m_historyScoreWeight);
}

QJsonDocument Launcher::defaultConfig() const
{
    QJsonObject rootObject;
    QJsonObject modulesObject;
    for (const ModuleConfig &config : m_moduleConfigs)
    {
        QJsonObject moduleObject;
        moduleObject["enabled"] = config.enabled;
        moduleObject["global"] = config.global;
        moduleObject["priority"] = config.priority;
        moduleObject["prefix"] = QString(config.prefix);
        modulesObject[ConfigManager::toCamelCase(config.name)] = moduleObject;
    }
    rootObject["modules"] = modulesObject;
    QJsonObject historyObject;
    historyObject["decay"] = m_decay;
    historyObject["minScore"] = m_minScore;
    historyObject["increment"] = m_increment;
    historyObject["historyScoreWeight"] = m_historyScoreWeight;
    rootObject["history"] = historyObject;
    QJsonObject uiObject;
    uiObject["maxVisibleResults"] = m_maxVisibleResults;
    rootObject["ui"] = uiObject;
    return QJsonDocument(rootObject);
}

/**
 * Use Windows API to register global Alt + Space hotkey.
 *
 * @return True if the hotkey is successfully registered; false otherwise.
 */
bool Launcher::registerHotkey() const
{
    connect(HotkeyManager::instance(), &HotkeyManager::hotkeyPressed, this, &Launcher::onHotkeyPressed);
    return HotkeyManager::registerHotkey(MOD_ALT, VK_SPACE, 0);
}

/**
 * Show or hide the main window.
 *
 * @param visibility The new window visibility (true to show; false to hide).
 */
void Launcher::setWindowVisibility(const bool &visibility)
{
    isWindowShown = visibility;
    if (!visibility)
    {
        m_searchEdit->clear();
        m_resultsList->clear();
        m_resultsList->hide();
        QApplication::processEvents(); // Force the event loop to process the above changes.
                                       // The stale bitmap cached by the window manager should be updated before hiding the window.
        hide();
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
    ThemeManager::initTheme();

    // Shadow effects.
    auto *searchFrameShadowEffect = new QGraphicsDropShadowEffect(this);
    searchFrameShadowEffect->setBlurRadius(SHADOW_BLUR_RADIUS);
    searchFrameShadowEffect->setColor(QColor::fromRgbF(0, 0, 0, SHADOW_OPACITY));
    searchFrameShadowEffect->setOffset(0, SHADOW_OFFSET_V);
    auto *resultsListShadowEffect = new QGraphicsDropShadowEffect(this);
    resultsListShadowEffect->setBlurRadius(SHADOW_BLUR_RADIUS);
    resultsListShadowEffect->setColor(QColor::fromRgbF(0, 0, 0, SHADOW_OPACITY));
    resultsListShadowEffect->setOffset(0, SHADOW_OFFSET_V);

    // Main layout.
    const int maxResultsListHeight = m_maxVisibleResults * (PADDING_S + PADDING_L + BUTTON_SIZE + PADDING_L) + PADDING_S;
    resize(WINDOW_MARGIN + WINDOW_WIDTH + WINDOW_MARGIN,
           WINDOW_MARGIN + PADDING_L + BUTTON_SIZE + PADDING_L + PADDING_L + maxResultsListHeight + PADDING_S + WINDOW_MARGIN);
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);
    m_mainLayout->setSpacing(PADDING_L);
    m_centralWidget->setLayout(m_mainLayout);

    // Search area.
    QFont iconFont;
    iconFont.setFamily("Material Symbols Rounded");
    iconFont.setPixelSize(ICON_SIZE);
    m_searchFrame = new QFrame(this);
    m_searchFrame->setFixedHeight(PADDING_S + PADDING_L + BUTTON_SIZE + PADDING_L + PADDING_S);
    m_searchFrame->setFixedWidth(WINDOW_WIDTH);
    m_searchFrame->setStyleSheet(
        QString("QFrame { border: none; border-radius: %1px; background-color: %2 }").arg(CORNER_RADIUS_L).arg(ThemeManager::defaultBackColorHex()));
    m_searchFrame->setGraphicsEffect(searchFrameShadowEffect);
    m_searchLayout = new QHBoxLayout(m_searchFrame);
    m_searchLayout->setContentsMargins(PADDING_S + PADDING_L, PADDING_S + PADDING_L, PADDING_S + PADDING_L, PADDING_S + PADDING_L);
    m_searchLayout->setSpacing(PADDING_L);
    m_searchIcon = new QLabel(this);
    m_searchIcon->setText(QChar(0xe8b6)); // Search.
    m_searchIcon->setFont(iconFont);
    m_searchIcon->setFixedWidth(BUTTON_SIZE);
    m_searchIcon->setFixedHeight(BUTTON_SIZE);
    m_searchIcon->setAlignment(Qt::AlignCenter);
    m_searchIcon->setStyleSheet(QString("QLabel { border: none; background: transparent; color: %1; }").arg(ThemeManager::defaultTextColorHex()));
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Start typing...");
    m_searchEdit->setFixedHeight(BUTTON_SIZE);
    m_searchEdit->setFocus();
    m_searchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_searchEdit->setStyleSheet(QString("QLineEdit { border: none; background: transparent; color: %1; font-size: %2px; padding: 0px; }")
                                    .arg(ThemeManager::defaultTextColorHex())
                                    .arg(TITLE_FONT_SIZE));
    connect(m_searchEdit, &QLineEdit::textChanged, this, &Launcher::onInputTextChanged);
    m_searchLayout->addWidget(m_searchIcon);
    m_searchLayout->addWidget(m_searchEdit);

    // Results list.
    m_resultsList = new QListWidget(this);
    m_resultsList->setFixedWidth(WINDOW_WIDTH);
    m_resultsList->setFixedHeight(maxResultsListHeight);
    m_resultsList->setFocusPolicy(Qt::NoFocus);
    m_resultsList->setMouseTracking(true);
    m_resultsList->setSpacing(PADDING_S / 2); // Set spacing and padding separately to keep the spacing between items and the list widget padding the same.
    m_resultsList->setStyleSheet(QString("QListWidget { border: none; border-radius: %1px; background-color: %2; padding: %3px; }")
                                     .arg(CORNER_RADIUS_L)
                                     .arg(ThemeManager::defaultBackColorHex())
                                     .arg(PADDING_S / 2));
    m_resultsList->setGraphicsEffect(resultsListShadowEffect);
    m_resultsList->hide();

    // Set custom delegate for results list.
    m_resultItemDelegate = new ResultItemDelegate(m_resultsList, this);
    m_resultsList->setItemDelegate(m_resultItemDelegate);
    connect(m_resultItemDelegate, &ResultItemDelegate::hideWindow, this, [&] { setWindowVisibility(false); });

    // Install event filter to handle keyboard navigation.
    m_searchEdit->installEventFilter(this);
    m_resultsList->installEventFilter(this);

    m_mainLayout->addWidget(m_searchFrame);
    m_mainLayout->addWidget(m_resultsList);
    m_mainLayout->addStretch(1);

    // Move to the center of screen.
    const QScreen *screen = QApplication::primaryScreen();
    const int screenWidth = screen->geometry().right() + 1;
    const int screenHeight = screen->geometry().bottom() + 1;
    move(screenWidth / 2 - width() / 2, screenHeight / 2 - height() / 2);
}

/**
 * Setup modules and connect to slots.
 */
void Launcher::setupModules()
{
    m_moduleConfigs = {
        ModuleConfig(new LauncherCommands(this), true, true, 1.0, ':'), //
        ModuleConfig(new EverythingSearch(this), true, false, 0.0, '@'), //
        ModuleConfig(new Calculator(this), true, true, 1.0, '='), //
        ModuleConfig(new AppsSearch(this), true, true, 0.8, ' '), //
        ModuleConfig(new SystemCommands(this), true, true, 1.0, ' '), //
        ModuleConfig(new WindowsTerminal(this), true, true, 1.0, '>'), //
        ModuleConfig(new UnitConverter(this), true, true, 1.0, ' ') //
    };

    // Connect all modules to results ready signal.
    for (ModuleConfig &config : m_moduleConfigs)
    {
        config.name = config.module->name();
        config.iconGlyph = config.module->iconGlyph();
        connect(config.module, &IModule::resultsReady, this, &Launcher::onResultsReady);
    }
}

void Launcher::readConfiguration()
{
    const QJsonDocument doc = ConfigManager::loadConfig(this);
    const QJsonObject rootObject = doc.object();
    const QJsonObject modulesObject = rootObject["modules"].toObject();
    for (ModuleConfig &config : m_moduleConfigs)
    {
        const QJsonObject moduleObject = modulesObject[ConfigManager::toCamelCase(config.name)].toObject();

        config.enabled = moduleObject["enabled"].toBool();
        config.global = moduleObject["global"].toBool();
        config.priority = moduleObject["priority"].toDouble();
        config.prefix = moduleObject["prefix"].toString(" ")[0]; // If prefix is not provided, use a space character.
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
    const QJsonObject uiObject = rootObject["ui"].toObject();
    m_maxVisibleResults = uiObject["maxVisibleResults"].toInt();
}

/**
 * Handle hotkey pressed signal from HotkeyManager.
 *
 * @param id The hotkey id.
 */
void Launcher::onHotkeyPressed(long long id)
{
    if (id == 0)
        setWindowVisibility(!isWindowShown);
}

/**
 * Handle results ready signal from modules.
 *
 * @param results The list of results to be displayed.
 * @param module The module providing the results.
 */
void Launcher::onResultsReady(QVector<ResultItem> &results, const IModule *module)
{
    double priority = 0.0;
    for (const ModuleConfig &config : m_moduleConfigs)
        if (config.module == module)
            priority = config.priority;

    for (auto &item : results)
    {
        item.priority = priority;
        const auto listItem = new ResultItemWidget(m_resultsList);
        listItem->setData(Qt::UserRole, QVariant::fromValue(item));
        m_resultsList->addItem(listItem);
    }

    m_resultsList->sortItems(Qt::DescendingOrder);

    if (m_resultsList->count() == 0)
    {
        m_resultsList->hide();
    }
    if (m_resultsList->count() > 0)
    {
        m_resultsList->show();
        m_resultsList->setFixedHeight(std::min(m_resultsList->count(), m_maxVisibleResults) * (PADDING_S + PADDING_L + BUTTON_SIZE + PADDING_L) + PADDING_S);
        m_resultsList->setCurrentRow(0);
        m_resultItemDelegate->setCurrentActionIndex(0);
    }
}

/**
 * Handle change of search text.
 *
 * @param text The search text.
 */
void Launcher::onInputTextChanged(const QString &text)
{
    m_resultsList->clear();
    m_searchIcon->setText(QChar(0xe8b6)); // Search.

    if (!text.isEmpty())
    {
        const QChar prefix = text.at(0);

        for (const ModuleConfig &config : m_moduleConfigs)
        {
            if (config.prefix == prefix && prefix != ' ')
            {
                m_searchIcon->setText(config.iconGlyph);
                config.module->query(text.mid(1).trimmed());
                return;
            }
        }

        for (const ModuleConfig &config : m_moduleConfigs)
        {
            if (config.global)
            {
                config.module->query(text.trimmed());
            }
        }
    }
    else
    {
        m_resultsList->hide();
    }
}

/**
 * Filter and handle specific key press events for the application.
 *
 * @param obj The object that generated the event.
 * @param event The event to be filtered.
 * @return True if the event is handled, otherwise the default event filter behavior is applied.
 */
bool Launcher::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const auto keyEvent = dynamic_cast<QKeyEvent *>(event);

        const QListWidgetItem *currentItem = m_resultsList->currentItem();
        if (!currentItem)
            return QMainWindow::eventFilter(obj, event);

        const QVariant data = currentItem->data(Qt::UserRole);
        const auto item = data.value<ResultItem>();

        if (keyEvent->modifiers() != Qt::NoModifier)
        {
            const QKeySequence pressedShortcut(keyEvent->modifiers() | keyEvent->key());
            if (executeShortcutAction(item, pressedShortcut))
            {
                return true;
            }
            return QMainWindow::eventFilter(obj, event);
        }

        // Navigate between actions.
        if (keyEvent->key() == Qt::Key_Tab)
        {
            handleActionsNavigation(item, true);
            return true;
        }
        if (keyEvent->key() == Qt::Key_Right)
        {
            if (m_searchEdit->cursorPosition() < m_searchEdit->text().length())
                return QMainWindow::eventFilter(obj, event);
            handleActionsNavigation(item, true);
            return true;
        }
        if (keyEvent->key() == Qt::Key_Left)
        {
            if (m_resultItemDelegate->getCurrentActionIndex() == 0)
                return QMainWindow::eventFilter(obj, event);
            handleActionsNavigation(item, false);
            return true;
        }

        // Launch current action.
        if (keyEvent->key() == Qt::Key_Return)
        {
            executeCurrentAction(item);
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
 * @param item The current result item.
 * @param right Whether to navigate right (true for right; false for left).
 */
void Launcher::handleActionsNavigation(const ResultItem &item, const bool &right) const
{
    if (item.actions.isEmpty())
        return;

    // Get current action index from delegate.
    const int currentIndex = m_resultItemDelegate->getCurrentActionIndex();
    const int actionCount = static_cast<int>(item.actions.size());
    const int newIndex = currentIndex + (right ? 1 : -1);

    if (newIndex >= 0 && newIndex < actionCount)
    {
        m_resultItemDelegate->setCurrentActionIndex(newIndex);

        // Force repaint to show the highlight.
        m_resultsList->viewport()->update();
    }
}

/**
 * Launch the action according to its shortcut assigned.
 *
 * @param item The current result item.
 * @param pressedShortcut The actual pressed shortcut.
 * @return True if an action is executed; false otherwise.
 */
bool Launcher::executeShortcutAction(const ResultItem &item, const QKeySequence &pressedShortcut)
{
    for (const auto &action : item.actions)
    {
        if (!action.shortcut.isEmpty() && action.shortcut == pressedShortcut)
        {
            if (action.handler)
                action.handler();
            if (!item.key.isEmpty())
                HistoryManager::addHistory(item.key);

            setWindowVisibility(false);
            return true;
        }
    }

    return false;
}

/**
 * Launch the highlighted action of the current result.
 *
 * @param item The current result item.
 */
void Launcher::executeCurrentAction(const ResultItem &item)
{
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
            HistoryManager::addHistory(item.key);
    }
}
