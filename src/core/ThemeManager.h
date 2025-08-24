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
    static QColor accentTextColor();

    static QString defaultBackColorHex();
    static QString activeBackColorHex();
    static QString accentBackColorHex();
    static QString defaultTextColorHex();
    static QString accentTextColorHex();

private:
    static inline bool m_isDarkMode;
};
