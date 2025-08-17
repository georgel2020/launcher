#pragma once

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
 *
 * An iconGlyph must be provided unless the action is the primary action.
 */
struct Action
{
    QChar iconGlyph;
    std::function<void()> handler;
};

Q_DECLARE_METATYPE(Action)
