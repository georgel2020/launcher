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
 */
struct Action
{
    QChar iconGlyph;
    std::function<void()> handler;
};

Q_DECLARE_METATYPE(Action)
