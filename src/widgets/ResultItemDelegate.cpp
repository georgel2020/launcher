#include "ResultItemDelegate.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QFontMetrics>
#include <QIcon>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QVariant>
#include "../common/Constants.h"
#include "../core/ThemeManager.h"

ResultItemDelegate::ResultItemDelegate(QAbstractItemView *view, QObject *parent) : QStyledItemDelegate(parent)
{
    // Store the abstract item view to trigger repaint.
    m_view = view;
}

/**
 * Paint a custom delegate item in a list widget.
 *
 * Override the base `QStyledItemDelegate::paint` method to provide
 * a customized rendering for the delegate item, including icons, text,
 * and action buttons. Handle selection and hover states with appropriate
 * visual indications.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param option The style options for the item.
 * @param index The QModelIndex representing the item being rendered.
 */
void ResultItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Get the ResultItem data.
    const QVariant data = index.data(Qt::UserRole);

    const auto item = data.value<ResultItem>();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    const bool isHovered = option.state & QStyle::State_MouseOver;
    const bool isSelected = option.state & QStyle::State_Selected;
    const bool isPrimaryHovered = isHovered && m_hoveredActionIndex == 0;
    const bool isPrimarySelected = isSelected && m_selectedActionIndex == 0;

    // Calculate rects for different components.
    const int visibleActionCount = (isSelected || isHovered) ? static_cast<int>(item.actions.size()) : 1; // Including the primary action.
    const QRect iconRect = getIconRect(option.rect);
    const QRect titleRect = getTitleRect(option.rect, visibleActionCount);
    const QRect subtitleRect = getSubtitleRect(option.rect, visibleActionCount);
    const QRect actionsRect = getActionsRect(option.rect, visibleActionCount);

    // Draw selection background if hovered or selected.
    if (isPrimaryHovered || isPrimarySelected)
    {
        QPainterPath path;
        path.addRoundedRect(option.rect, CORNER_RADIUS_M, CORNER_RADIUS_M);
        painter->setPen(Qt::NoPen);
        painter->fillPath(path, isPrimarySelected ? ThemeManager::accentBackColor() : ThemeManager::activeBackColor());
        painter->drawPath(path);
    }

    // Draw icon.
    if (item.iconGlyph != QChar())
    {
        // Draw font icon.
        drawIconGlyph(painter, iconRect, item.iconGlyph, isPrimarySelected ? ThemeManager::accentTextColor() : ThemeManager::defaultTextColor());
    }
    else if (item.iconPath != QString())
    {
        // Draw icon from the given file.
        const QFileIconProvider iconProvider;
        const QFileInfo iconInfo(item.iconPath);
        const QIcon fileIcon = iconProvider.icon(iconInfo);
        drawIcon(painter, iconRect, fileIcon);
    }

    // Draw title.
    QFont titleFont = option.font;
    titleFont.setBold(true);
    titleFont.setPixelSize(TITLE_FONT_SIZE);
    drawText(painter, titleRect, item.title, titleFont, isPrimarySelected ? ThemeManager::accentTextColor() : ThemeManager::defaultTextColor());

    // Draw subtitle.
    QFont subtitleFont = option.font;
    subtitleFont.setPixelSize(SUBTITLE_FONT_SIZE);
    drawText(painter, subtitleRect, item.subtitle, subtitleFont, isPrimarySelected ? ThemeManager::accentTextColor() : ThemeManager::defaultTextColor());

    // Draw action buttons.
    if (isSelected || isHovered) // Action buttons are hidden by default.
        drawActionButtons(painter, actionsRect, item.actions, m_selectedActionIndex, m_hoveredActionIndex, isSelected, isHovered);

    painter->restore();
}

/**
 * Calculate the size hint for a custom delegate list item.
 *
 * Override the base `QStyledItemDelegate::sizeHint` method to provide
 * the preferred size for the delegate item.
 *
 * @param option The style options for the item.
 * @param index The QModelIndex representing the item whose size
 * is being calculated.
 * @return The QSize object specifying the preferred dimensions
 * for the delegate item.
 */
QSize ResultItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return {-1, PADDING_M + BUTTON_SIZE + PADDING_M};
}

/**
 * Handle editor events for the delegate, allowing for interaction with custom action buttons.
 *
 * Process mouse events, particularly mouse button press and release events.
 * Dynamically detect whether a click occurs on one of the action buttons within the
 * delegate, and if so, execute the corresponding action handler.
 *
 * @param event The event object representing the user interaction.
 * @param model The model associated with the view.
 * @param option The style options for the item.
 * @param index The QModelIndex of the item that the event corresponds to.
 * @return True if the action was successfully processed; otherwise, false,
 * meaning the event will fall back to the base implementation.
 */
bool ResultItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() != QEvent::MouseMove && event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonDblClick)
        return QStyledItemDelegate::editorEvent(event, model, option, index);

    const auto mouseEvent = dynamic_cast<QMouseEvent *>(event);
    if (event->type() != QEvent::MouseMove && mouseEvent->button() != Qt::LeftButton)
        return QStyledItemDelegate::editorEvent(event, model, option, index);

    // Get the ResultItem data.
    const QVariant data = index.data(Qt::UserRole);
    auto item = data.value<ResultItem>();

    const QRect actionsRect = getActionsRect(option.rect, static_cast<int>(item.actions.size()));
    const int buttonIndex = getActionButtonIndex(mouseEvent->pos(), actionsRect, static_cast<int>(item.actions.size()));

    if (event->type() == QEvent::MouseMove)
    {
        m_hoveredActionIndex = buttonIndex;
        m_view->viewport()->update();
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    if ((buttonIndex >= 1 && buttonIndex < item.actions.size() && event->type() == QEvent::MouseButtonPress) ||
        (buttonIndex == 0 && event->type() == QEvent::MouseButtonDblClick))
    {
        if (item.actions[buttonIndex].handler)
        {
            item.actions[buttonIndex].handler();
            emit hideWindow();
            return true;
        }

        emit hideWindow();
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

/**
 * Get the focused action index.
 *
 * @return An integer representing the action index.
 */
int ResultItemDelegate::getCurrentActionIndex() const { return m_selectedActionIndex; }

/**
 * Set the focused action index.
 *
 * @param index An integer representing the action index
 */
void ResultItemDelegate::setCurrentActionIndex(const int index) const { m_selectedActionIndex = index; }

/**
 * Draw a QIcon at the given location.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param rect The QRect specifying where to paint the icon.
 * @param icon The QIcon to be rendered.
 */
void ResultItemDelegate::drawIcon(QPainter *painter, const QRect &rect, const QIcon &icon)
{
    if (!icon.isNull())
    {
        icon.paint(painter, rect, Qt::AlignCenter);
    }
}

/**
 * Draw a font icon at the given location.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param rect The QRect specifying where to paint the icon.
 * @param icon The QIcon to be rendered.
 * @param color The color of the icon.
 */
void ResultItemDelegate::drawIconGlyph(QPainter *painter, const QRect &rect, const QChar &icon, const QColor &color)
{
    QFont iconFont;
    iconFont.setFamily("Material Symbols Rounded");
    iconFont.setPixelSize(ICON_SIZE);
    painter->setFont(iconFont);
    painter->setPen(color);
    painter->drawText(rect, Qt::AlignCenter, QString(icon));
}

/**
 * Draw a line of text at the given location.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param rect The QRect specifying where to paint the text.
 * @param text The text to be rendered.
 * @param font The font of the text.
 * @param color The color of the text.
 */
void ResultItemDelegate::drawText(QPainter *painter, const QRect &rect, const QString &text, const QFont &font, const QColor &color)
{
    const QFontMetrics metrics(font);
    const QString elidedText = metrics.elidedText(text, Qt::ElideRight, rect.width());
    painter->setFont(font);
    painter->setPen(color);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
}

/**
 * Draw action buttons at the given location.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param rect The QRect specifying where to paint the action buttons.
 * @param actions The action button structures.
 * @param currentActionIndex The current action index.
 * @param hoveredActionIndex The hovered action index.
 * @param isSelected Whether the current result item is selected.
 * @param isHovered Whether the current result item is hovered.
 */
void ResultItemDelegate::drawActionButtons(QPainter *painter, const QRect &rect, const QVector<Action> &actions, const int &currentActionIndex,
                                           const int &hoveredActionIndex, const bool &isSelected, const bool &isHovered)
{
    if (actions.size() == 1) // Only one primary action.
        return;

    const int actionCount = static_cast<int>(actions.size());

    for (int actionIndex = actionCount - 1; actionIndex >= 1; actionIndex--) // The first action is the primary action.
    {
        const int buttonX = rect.right() - (actionCount - actionIndex) * (BUTTON_SIZE + PADDING_M);
        const QRect buttonRect(buttonX, rect.top() + PADDING_M, BUTTON_SIZE, BUTTON_SIZE);

        painter->setPen(Qt::NoPen);

        // Determine background color based on state
        QColor backgroundColor;
        if (isSelected && currentActionIndex == actionIndex)
            backgroundColor = ThemeManager::accentBackColor();
        else if (isHovered && hoveredActionIndex == actionIndex)
            backgroundColor = ThemeManager::activeBackColor();
        else
            backgroundColor = ThemeManager::defaultBackColor();

        painter->setBrush(backgroundColor);
        painter->drawRoundedRect(buttonRect, CORNER_RADIUS_S, CORNER_RADIUS_S);

        // Paint the icon separately to control the size.
        drawIconGlyph(painter, buttonRect, actions[actionIndex].iconGlyph,
                      (isSelected && currentActionIndex == actionIndex) ? ThemeManager::accentTextColor() : ThemeManager::defaultTextColor());
    }
}

/**
 * Calculate the QRect for rendering an icon within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the icon will be displayed.
 * @return A QRect object representing the calculated position and size of the icon.
 */
QRect ResultItemDelegate::getIconRect(const QRect &itemRect)
{
    return {
        itemRect.left() + PADDING_M + (BUTTON_SIZE - ICON_SIZE) / 2, //
        itemRect.center().y() - ICON_SIZE / 2, //
        ICON_SIZE, //
        ICON_SIZE //
    };
}

/**
 * Calculate the QRect for rendering a title within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the title will be displayed.
 * @param actionsCount The count of actions, including the primary action.
 * @return A QRect object representing the calculated position and size of the title.
 */
QRect ResultItemDelegate::getTitleRect(const QRect &itemRect, const int &actionsCount)
{
    constexpr int leftMargin = PADDING_M + BUTTON_SIZE + PADDING_M;
    constexpr int topMargin = PADDING_M;
    return {
        itemRect.left() + leftMargin, //
        itemRect.top() + topMargin, //
        itemRect.width() - leftMargin - getActionsRect(itemRect, actionsCount).width(), //
        itemRect.height() / 2 - topMargin //
    };
}

/**
 * Calculate the QRect for rendering a subtitle within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the subtitle will be displayed.
 * @param actionsCount The count of actions, including the primary action.
 * @return A QRect object representing the calculated position and size of the subtitle.
 */
QRect ResultItemDelegate::getSubtitleRect(const QRect &itemRect, const int &actionsCount)
{
    constexpr int leftMargin = PADDING_M + BUTTON_SIZE + PADDING_M;
    const int topPosition = itemRect.top() + itemRect.height() / 2;
    return {
        itemRect.left() + leftMargin, //
        topPosition, //
        itemRect.width() - leftMargin - getActionsRect(itemRect, actionsCount).width(), //
        itemRect.height() / 2 - PADDING_M //
    };
}

/**
 * Calculate the QRect for rendering action buttons within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the actions will be displayed.
 * @param actionsCount The count of actions, including the primary action.
 * @return A QRect object representing the calculated position and size of the action buttons.
 */
QRect ResultItemDelegate::getActionsRect(const QRect &itemRect, const int &actionsCount)
{
    const int actionsWidth = (actionsCount - 1) * (BUTTON_SIZE + PADDING_M) + PADDING_M; // The primary action is not displayed as a button.

    return {
        itemRect.right() - actionsWidth + 2, // For perfect button alignment.
        itemRect.top(), //
        actionsWidth, //
        itemRect.height() //
    };
}

/**
 * Determine the index of an action button based on the provided position.
 *
 * Calculates the index of an action button within a specified rectangle
 * containing multiple buttons.
 *
 * @param pos The QPoint representing the position to check, in item rectangle coordinates.
 * @param actionsRect The QRect representing the bounding rectangle of the action buttons.
 * @param actionCount The total number of action buttons within the rectangle.
 * @return The zero-based index of the action if found, or 0 if no valid action is found.
 */
int ResultItemDelegate::getActionButtonIndex(const QPoint &pos, const QRect &actionsRect, const int &actionCount)
{
    if (!actionsRect.contains(pos) || actionCount == 0)
    {
        return 0;
    }

    const int startX = actionsRect.left() + PADDING_M;
    const int relativeX = pos.x() - startX;
    const int relativeY = pos.y() - actionsRect.top();

    if (relativeX < 0)
        return 0;
    if (relativeY < PADDING_M || relativeY > PADDING_M + BUTTON_SIZE)
        return 0;

    const int buttonIndex = relativeX / (BUTTON_SIZE + PADDING_M);
    const int buttonOffset = relativeX % (BUTTON_SIZE + PADDING_M);

    if (buttonOffset < BUTTON_SIZE && buttonIndex < actionCount)
        return buttonIndex + 1; // The first action is not a button.

    return 0;
}
