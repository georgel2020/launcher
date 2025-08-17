#pragma once

#include <QString>
#include <QVector>
#include "Action.h"

/**
 * @struct ResultItem
 * @brief Represent an item in a result view.
 *
 * Members:
 *
 * - title: The main title representing the item.
 * - subtitle: A secondary description providing additional context.
 * - iconGlyph: A single character representing an icon in glyph form.
 * - iconPath: The file path to an external icon source, such as an executable file.
 * - actions: A collection of actions that the user can perform on this item.
 *
 * An icon must be provided, either as a font icon or a path.
 * The action list can be empty.
 */
struct ResultItem
{
    QString title;
    QString subtitle;
    QChar iconGlyph;
    QString iconPath;
    QVector<Action> actions;
};

Q_DECLARE_METATYPE(ResultItem)
