#pragma once

#include <QObject>

class ThemeManager final
{
public:
    ThemeManager() = delete;

    static void initTheme();

    static QJsonDocument defaultConfig();

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

    static inline QString m_defaultBackLight, m_defaultBackDark;
    static inline QString m_activeBackLight, m_activeBackDark;
    static inline QString m_accentBackLight, m_accentBackDark;
    static inline QString m_defaultTextLight, m_defaultTextDark;
    static inline QString m_accentTextLight, m_accentTextDark;
};
