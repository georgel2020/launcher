#pragma once

#include <QString>
#include <QMetaType>
#include <functional>

/**
 * @struct Action
 * @brief Represent an action that can be performed.
 *
 * Members:
 *
 * - name: The name of the action.
 * - iconGlyph: A single character representing an icon in glyph form.
 * - handler: The action handler to be called when triggered.
 */
struct Action {
    QString name;
    QChar iconGlyph;
    std::function<void()> handler;
};

Q_DECLARE_METATYPE(Action)