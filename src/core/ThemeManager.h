#pragma once

#include <QObject>

class ThemeManager final
{
public:
    ThemeManager() = delete;

    static void initTheme();

    static QString primaryBackColorHex();
    static QString secondaryBackColorHex();
    static QString primaryTextColorHex();
    static QString secondaryTextColorHex();

    static QColor primaryBackColor();
    static QColor secondaryBackColor();
    static QColor primaryTextColor();
    static QColor secondaryTextColor();

private:
    static inline bool m_isDarkMode;
};
