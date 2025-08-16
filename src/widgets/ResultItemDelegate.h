#pragma once

#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QVariant>
#include "../common/Action.h"
#include "../common/Constants.h"
#include "../common/ResultItem.h"

class ResultItemDelegate final : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ResultItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    [[nodiscard]] int getCurrentActionIndex() const;
    void setCurrentActionIndex(int index);

private:
    static void drawIcon(QPainter* painter, const QRect& rect, const QIcon& icon);
    static void drawIconGlyph(QPainter* painter, const QRect& rect, const QChar& icon, const QColor& color);
    static void drawText(QPainter* painter, const QRect& rect, const QString& text, const QFont& font, const QColor& color);
    static void drawActionButtons(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, const QVector<Action>& actions, const QColor& color,
                                  int currentActionIndex = -1);

    [[nodiscard]] static QRect getIconRect(const QRect& itemRect);
    [[nodiscard]] static QRect getTitleRect(const QRect& itemRect, const int& actionsCount);
    [[nodiscard]] static QRect getSubtitleRect(const QRect& itemRect, const int& actionsCount);
    [[nodiscard]] static QRect getActionsRect(const QRect& itemRect, const int& actionsCount);

    int m_currentActionIndex = 0;
};
