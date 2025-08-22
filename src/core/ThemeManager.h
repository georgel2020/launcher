#pragma once

#include <QObject>

class ThemeManager final
{
public:
    ThemeManager() = delete;

    static void initTheme();

    static QColor defaultBackColor();
    static QColor activeBackColor();
    static QColor accentBackColor();
    static QColor defaultTextColor();

    static QString defaultBackColorHex();
    static QString activeBackColorHex();
    static QString accentBackColorHex();
    static QString defaultTextColorHex();

private:
    static inline bool m_isDarkMode;
};
