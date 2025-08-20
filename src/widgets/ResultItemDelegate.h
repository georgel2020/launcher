#pragma once

#include <QStyledItemDelegate>
#include "../common/Action.h"
#include "../common/ResultItem.h"

class ResultItemDelegate final : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ResultItemDelegate(QAbstractItemView *view, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    [[nodiscard]] int getCurrentActionIndex() const;
    void setCurrentActionIndex(int index) const;

private:
    static void drawIcon(QPainter *painter, const QRect &rect, const QIcon &icon);
    static void drawIconGlyph(QPainter *painter, const QRect &rect, const QChar &icon);
    static void drawText(QPainter *painter, const QRect &rect, const QString &text, const QFont &font);
    static void drawActionButtons(QPainter *painter, const QRect &rect, const QVector<Action> &actions,
                                  const int &currentActionIndex = 0, const int &hoveredActionIndex = 0, const bool &isSelected = false,
                                  const bool &isHovered = false);

    [[nodiscard]] static QRect getIconRect(const QRect &itemRect);
    [[nodiscard]] static QRect getTitleRect(const QRect &itemRect, const int &actionsCount);
    [[nodiscard]] static QRect getSubtitleRect(const QRect &itemRect, const int &actionsCount);
    [[nodiscard]] static QRect getActionsRect(const QRect &itemRect, const int &actionsCount);
    [[nodiscard]] static int getActionButtonIndex(const QPoint &pos, const QRect &actionsRect, const int &actionCount);

    QAbstractItemView *m_view;

    mutable int m_currentActionIndex = 0;
    mutable int m_hoveredActionIndex = 0;

signals:
    void hideWindow();
};
