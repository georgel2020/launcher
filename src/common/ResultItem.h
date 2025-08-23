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
 * - key: A unique string allocated to the result item.
 * - priority: The module priority; should not be assigned by the module.
 * - score: The score of the result (1.0 by default).
 *
 * An icon must be provided, either as a font icon or a path.
 * The action list can be empty.
 * If the key is not provided, the result item will not be written into run history.
 */
struct ResultItem
{
    QString title;
    QString subtitle;
    QChar iconGlyph;
    QString iconPath;
    QVector<Action> actions;
    QString key;
    double priority = 1.0;
    double score = 1.0;
};

Q_DECLARE_METATYPE(ResultItem)
