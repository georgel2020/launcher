#include "ResultItemDelegate.h"
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

ResultItemDelegate::ResultItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

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

    auto [title, subtitle, iconGlyph, iconPath, actions] = data.value<ResultItem>();

    painter->save();

    bool isSelected = option.state & QStyle::State_Selected;

    // Draw selection background if hovered or selected.
    if (isSelected)
    {
        QPainterPath path;
        path.addRoundedRect(option.rect, CORNER_RADIUS, CORNER_RADIUS);
        painter->setPen(Qt::NoPen);
        painter->fillPath(path, m_currentActionIndex == 0 ? option.palette.highlight() : option.palette.alternateBase());
        painter->drawPath(path);
    }

    // Calculate rects for different components.
    const int visibleActionCount = isSelected ? static_cast<int>(actions.size()) : 1; // Including the primary action.
    QRect iconRect = getIconRect(option.rect);
    QRect titleRect = getTitleRect(option.rect, visibleActionCount);
    QRect subtitleRect = getSubtitleRect(option.rect, visibleActionCount);
    QRect actionsRect = getActionsRect(option.rect, visibleActionCount);

    // Draw icon.
    if (iconGlyph != QChar())
    {
        // Draw font icon.
        drawIconGlyph(painter, iconRect, iconGlyph, option.palette.text().color());
    }
    else if (iconPath != QString())
    {
        // Draw icon from the given executable file.
        QFileIconProvider iconProvider;
        QFileInfo iconInfo(iconPath);
        QIcon fileIcon = iconProvider.icon(iconInfo);
        drawIcon(painter, iconRect, fileIcon);
    }

    // Draw title.
    QFont titleFont = option.font;
    titleFont.setBold(true);
    titleFont.setPixelSize(TITLE_FONT_SIZE);
    QColor titleColor = (option.state & QStyle::State_Selected) ? option.palette.highlightedText().color() : option.palette.text().color();
    drawText(painter, titleRect, title, titleFont, titleColor);

    // Draw subtitle.
    QFont subtitleFont = option.font;
    subtitleFont.setPixelSize(SUBTITLE_FONT_SIZE);
    QColor subtitleColor = (option.state & QStyle::State_Selected) ? option.palette.highlightedText().color() : option.palette.text().color();
    drawText(painter, subtitleRect, subtitle, subtitleFont, subtitleColor);

    // Draw action buttons.
    if (isSelected) // Action buttons are hidden by default.
        drawActionButtons(painter, option, actionsRect, actions, option.palette.text().color(), m_currentActionIndex);

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
    return {-1, RESULT_PADDING + BUTTON_SIZE + RESULT_PADDING};
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
void ResultItemDelegate::setCurrentActionIndex(const int index) { m_currentActionIndex = index; }

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
    painter->setFont(font);
    painter->setPen(color);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

/**
 * Draw action buttons at the given location.
 *
 * @param painter The QPainter object used for rendering the item.
 * @param option The style options for the item.
 * @param rect The QRect specifying where to paint the action buttons.
 * @param actions The action button structures.
 * @param color The icon foreground color.
 * @param currentActionIndex The currently focused action index.
 */
void ResultItemDelegate::drawActionButtons(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, const QVector<Action>& actions,
                                           const QColor& color, const int currentActionIndex)
{
    if (actions.size() == 1) // Only one primary action.
        return;

    const int actionCount = static_cast<int>(actions.size());

    for (int actionIndex = actionCount - 1; actionIndex >= 1; actionIndex--) // The first action is the primary action.
    {
        const int buttonX = rect.right() - (actionCount - actionIndex) * (BUTTON_SIZE + RESULT_PADDING);
        const QRect buttonRect(buttonX, rect.top() + RESULT_PADDING, BUTTON_SIZE, BUTTON_SIZE);

        painter->setPen(Qt::NoPen);
        painter->setBrush(currentActionIndex == actionIndex ? option.palette.highlight() : option.palette.base());
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
    return {itemRect.left() + RESULT_PADDING + (BUTTON_SIZE - ICON_SIZE) / 2, itemRect.center().y() - ICON_SIZE / 2, ICON_SIZE, ICON_SIZE};
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
    constexpr int leftMargin = RESULT_PADDING + BUTTON_SIZE + RESULT_PADDING;
    constexpr int topMargin = RESULT_PADDING;
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
    constexpr int leftMargin = RESULT_PADDING + BUTTON_SIZE + RESULT_PADDING;
    const int topPosition = itemRect.top() + itemRect.height() / 2;
    return {itemRect.left() + leftMargin, topPosition, itemRect.width() - leftMargin - getActionsRect(itemRect, actionsCount).width(),
            itemRect.height() / 2 - RESULT_PADDING};
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
    const int actionsWidth = (actionsCount - 1) * (BUTTON_SIZE + RESULT_PADDING) + RESULT_PADDING; // The primary action is not displayed as a button.

    return {itemRect.right() - actionsWidth + 2, // For perfect button alignment.
            itemRect.top(), actionsWidth, itemRect.height()};
}
