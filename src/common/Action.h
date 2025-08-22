#pragma once

#include <QKeySequence>
#include <QMetaType>
#include <QString>
#include <functional>

/**
 * @struct Action
 * @brief Represent an action that can be performed.
 *
 * Members:
 *
 * - iconGlyph: A single character representing an icon in glyph form.
 * - handler: The action handler to be called when triggered.
 * - shortcut: An optional shortcut to directly trigger the action.
 *
 * An iconGlyph must be provided unless the action is the primary action.
 */
struct Action
{
    QChar iconGlyph;
    std::function<void()> handler;
    QKeySequence shortcut;
};

Q_DECLARE_METATYPE(Action)
