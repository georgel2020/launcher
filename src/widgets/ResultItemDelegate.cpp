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
#include <QPalette>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QVariant>

ResultItemDelegate::ResultItemDelegate(QAbstractItemView* view, QObject* parent) : QStyledItemDelegate(parent)
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
void ResultItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Get the ResultItem data.
    QVariant data = index.data(Qt::UserRole);

    auto item = data.value<ResultItem>();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    const bool isHovered = option.state & QStyle::State_MouseOver;
    const bool isSelected = option.state & QStyle::State_Selected;

    // Draw selection background if hovered or selected.
    if (isSelected)
    {
        QPainterPath path;
        path.addRoundedRect(option.rect, CORNER_RADIUS, CORNER_RADIUS);
        painter->setPen(Qt::NoPen);
        painter->fillPath(path, m_currentActionIndex == 0 ? option.palette.highlight() : option.palette.alternateBase());
        painter->drawPath(path);
    }
    if (isHovered && !isSelected && m_currentActionIndex == 0)
    {
        QPainterPath path;
        path.addRoundedRect(option.rect, CORNER_RADIUS, CORNER_RADIUS);
        painter->setPen(Qt::NoPen);
        painter->fillPath(path, option.palette.alternateBase());
        painter->drawPath(path);
    }

    // Calculate rects for different components.
    const int visibleActionCount =
        (isSelected || isHovered) ? static_cast<int>(item.actions.size()) : 1; // Including the primary action.
                                                                               // Without action buttons, the space is left to the text.
    QRect iconRect = getIconRect(option.rect);
    QRect titleRect = getTitleRect(option.rect, visibleActionCount);
    QRect subtitleRect = getSubtitleRect(option.rect, visibleActionCount);
    QRect actionsRect = getActionsRect(option.rect, visibleActionCount);

    // Draw icon.
    if (item.iconGlyph != QChar())
    {
        // Draw font icon.
        drawIconGlyph(painter, iconRect, item.iconGlyph, option.palette.text().color());
    }
    else if (item.iconPath != QString())
    {
        // Draw icon from the given file.
        QFileIconProvider iconProvider;
        QFileInfo iconInfo(item.iconPath);
        QIcon fileIcon = iconProvider.icon(iconInfo);
        drawIcon(painter, iconRect, fileIcon);
    }

    // Draw title.
    QFont titleFont = option.font;
    titleFont.setBold(true);
    titleFont.setPixelSize(TITLE_FONT_SIZE);
    QColor titleColor = (option.state & QStyle::State_Selected) ? option.palette.highlightedText().color() : option.palette.text().color();
    drawText(painter, titleRect, item.title, titleFont, titleColor);

    // Draw subtitle.
    QFont subtitleFont = option.font;
    subtitleFont.setPixelSize(SUBTITLE_FONT_SIZE);
    QColor subtitleColor = (option.state & QStyle::State_Selected) ? option.palette.highlightedText().color() : option.palette.text().color();
    drawText(painter, subtitleRect, item.subtitle, subtitleFont, subtitleColor);

    // Draw action buttons.
    if (isSelected || isHovered) // Action buttons are hidden by default.
        drawActionButtons(painter, option, actionsRect, item.actions, option.palette.text().color(), m_currentActionIndex, m_hoveredActionIndex, isSelected,
                          isHovered);

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
QSize ResultItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return {-1, PADDING + BUTTON_SIZE + PADDING};
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
bool ResultItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() != QEvent::MouseMove && event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonDblClick)
        return QStyledItemDelegate::editorEvent(event, model, option, index);

    const auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
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
int ResultItemDelegate::getCurrentActionIndex() const { return m_currentActionIndex; }

/**
 * Set the focused action index.
 *
 * @param index An integer representing the action index
 */
void ResultItemDelegate::setCurrentActionIndex(const int index) const { m_currentActionIndex = index; }

/**
 * Draw a QIcon at the given location.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param rect The QRect specifying where to paint the icon.
 * @param icon The QIcon to be rendered.
 */
void ResultItemDelegate::drawIcon(QPainter* painter, const QRect& rect, const QIcon& icon)
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
 * @param color The icon foreground color.
 */
void ResultItemDelegate::drawIconGlyph(QPainter* painter, const QRect& rect, const QChar& icon, const QColor& color)
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
 * @param color The icon foreground color.
 */
void ResultItemDelegate::drawText(QPainter* painter, const QRect& rect, const QString& text, const QFont& font, const QColor& color)
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
 * @param option The style options for the item.
 * @param rect The QRect specifying where to paint the action buttons.
 * @param actions The action button structures.
 * @param color The icon foreground color.
 * @param currentActionIndex The current action index.
 * @param hoveredActionIndex The hovered action index.
 * @param isSelected Whether the current result item is selected.
 * @param isHovered Whether the current result item is hovered.
 */
void ResultItemDelegate::drawActionButtons(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, const QVector<Action>& actions,
                                           const QColor& color, const int& currentActionIndex, const int& hoveredActionIndex, const bool& isSelected,
                                           const bool& isHovered)
{
    if (actions.size() == 1) // Only one primary action.
        return;

    const int actionCount = static_cast<int>(actions.size());

    for (int actionIndex = actionCount - 1; actionIndex >= 1; actionIndex--) // The first action is the primary action.
    {
        const int buttonX = rect.right() - (actionCount - actionIndex) * (BUTTON_SIZE + PADDING);
        const QRect buttonRect(buttonX, rect.top() + PADDING, BUTTON_SIZE, BUTTON_SIZE);

        painter->setPen(Qt::NoPen);

        // Determine background color based on state
        QColor backgroundColor;
        if (currentActionIndex == actionIndex && isSelected)
            backgroundColor = option.palette.highlight().color();
        else if (hoveredActionIndex == actionIndex && isHovered)
            backgroundColor = option.palette.alternateBase().color();
        else
            backgroundColor = option.palette.base().color();

        painter->setBrush(backgroundColor);
        painter->drawRoundedRect(buttonRect, CORNER_RADIUS, CORNER_RADIUS);

        // Paint the icon separately to control the size.
        drawIconGlyph(painter, buttonRect, actions[actionIndex].iconGlyph, color);
    }
}

/**
 * Calculate the QRect for rendering an icon within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the icon will be displayed.
 * @return A QRect object representing the calculated position and size of the icon.
 */
QRect ResultItemDelegate::getIconRect(const QRect& itemRect)
{
    return {itemRect.left() + PADDING + (BUTTON_SIZE - ICON_SIZE) / 2, itemRect.center().y() - ICON_SIZE / 2, ICON_SIZE, ICON_SIZE};
}

/**
 * Calculate the QRect for rendering a title within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the title will be displayed.
 * @param actionsCount The count of actions, including the primary action.
 * @return A QRect object representing the calculated position and size of the title.
 */
QRect ResultItemDelegate::getTitleRect(const QRect& itemRect, const int& actionsCount)
{
    constexpr int leftMargin = PADDING + BUTTON_SIZE + PADDING;
    constexpr int topMargin = PADDING;
    return {itemRect.left() + leftMargin, itemRect.top() + topMargin, itemRect.width() - leftMargin - getActionsRect(itemRect, actionsCount).width(),
            itemRect.height() / 2 - topMargin};
}

/**
 * Calculate the QRect for rendering a subtitle within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the subtitle will be displayed.
 * @param actionsCount The count of actions, including the primary action.
 * @return A QRect object representing the calculated position and size of the subtitle.
 */
QRect ResultItemDelegate::getSubtitleRect(const QRect& itemRect, const int& actionsCount)
{
    constexpr int leftMargin = PADDING + BUTTON_SIZE + PADDING;
    const int topPosition = itemRect.top() + itemRect.height() / 2;
    return {itemRect.left() + leftMargin, topPosition, itemRect.width() - leftMargin - getActionsRect(itemRect, actionsCount).width(),
            itemRect.height() / 2 - PADDING};
}

/**
 * Calculate the QRect for rendering action buttons within a list item.
 *
 * @param itemRect The bounding rectangle of the item in which the actions will be displayed.
 * @param actionsCount The count of actions, including the primary action.
 * @return A QRect object representing the calculated position and size of the action buttons.
 */
QRect ResultItemDelegate::getActionsRect(const QRect& itemRect, const int& actionsCount)
{
    const int actionsWidth = (actionsCount - 1) * (BUTTON_SIZE + PADDING) + PADDING; // The primary action is not displayed as a button.

    return {itemRect.right() - actionsWidth + 2, // For perfect button alignment.
            itemRect.top(), actionsWidth, itemRect.height()};
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
 * @return The zero-based index of the button if found, or 0 if no valid button is found
 * (the mouse is on the primary action area).
 */
int ResultItemDelegate::getActionButtonIndex(const QPoint& pos, const QRect& actionsRect, const int& actionCount)
{
    if (!actionsRect.contains(pos) || actionCount == 0)
    {
        return 0;
    }

    const int startX = actionsRect.left() + PADDING;
    const int relativeX = pos.x() - startX;
    const int relativeY = pos.y() - actionsRect.top();

    if (relativeX < 0)
        return 0;
    if (relativeY < PADDING || relativeY > PADDING + BUTTON_SIZE)
        return 0;

    const int buttonIndex = relativeX / (BUTTON_SIZE + PADDING);
    const int buttonOffset = relativeX % (BUTTON_SIZE + PADDING);

    if (buttonOffset < BUTTON_SIZE && buttonIndex < actionCount)
    {
        return buttonIndex + 1; // The first action is not a button.
    }

    return 0;
}
