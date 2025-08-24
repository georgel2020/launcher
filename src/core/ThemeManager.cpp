#include "ThemeManager.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStyleHints>
#include "../common/Constants.h"
#include "ConfigManager.h"

void ThemeManager::initTheme()
{
    const QJsonDocument doc = ConfigManager::loadConfig("Theme.json", defaultConfig());
    const QJsonObject rootObject = doc.object();
    const QString theme = rootObject["theme"].toString();
    if (theme == "auto")
        m_isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    else if (theme == "light")
        m_isDarkMode = false;
    else if (theme == "dark")
        m_isDarkMode = true;
    else
        m_isDarkMode = false;
    const QJsonObject colorsObject = rootObject["colors"].toObject();
    const QJsonObject lightObject = colorsObject["light"].toObject();
    const QJsonObject darkObject = colorsObject["dark"].toObject();
    m_defaultBackLight = lightObject["background"].toObject()["default"].toString();
    m_defaultBackDark = darkObject["background"].toObject()["default"].toString();
    m_activeBackLight = lightObject["background"].toObject()["active"].toString();
    m_activeBackDark = darkObject["background"].toObject()["active"].toString();
    m_accentBackLight = lightObject["background"].toObject()["accent"].toString();
    m_accentBackDark = darkObject["background"].toObject()["accent"].toString();
    m_defaultTextLight = lightObject["text"].toObject()["default"].toString();
    m_defaultTextDark = darkObject["text"].toObject()["default"].toString();
    m_accentTextLight = lightObject["text"].toObject()["accent"].toString();
    m_accentTextDark = darkObject["text"].toObject()["accent"].toString();
}

QJsonDocument ThemeManager::defaultConfig()
{
    // clang-format off
    const QJsonObject rootObject{
        {"theme", "auto"},
        {"colors", QJsonObject{
            {"light", QJsonObject{
                {"background", QJsonObject{
                    {"default", "#e3eae7"},
                    {"active",  "#cae6df"},
                    {"accent",  "#006b60"}
                }},
                {"text", QJsonObject{
                    {"default", "#171d1b"},
                    {"accent",  "#ffffff"}
                }}
            }},
            {"dark", QJsonObject{
                {"background", QJsonObject{
                    {"default", "#252b2a"},
                    {"active",  "#334b47"},
                    {"accent",  "#82d5c7"}
                }},
                {"text", QJsonObject{
                    {"default", "#dde4e1"},
                    {"accent",  "#003731"}
                }}
            }}
        }}
    };
    // clang-format on

    return QJsonDocument(rootObject);
}

QColor ThemeManager::defaultBackColor() { return QColor::fromString(m_isDarkMode ? m_defaultBackDark : m_defaultBackLight); }
QColor ThemeManager::activeBackColor() { return QColor::fromString(m_isDarkMode ? m_activeBackDark : m_activeBackLight); }
QColor ThemeManager::accentBackColor() { return QColor::fromString(m_isDarkMode ? m_accentBackDark : m_accentBackLight); }
QColor ThemeManager::defaultTextColor() { return QColor::fromString(m_isDarkMode ? m_defaultTextDark : m_defaultTextLight); }
QColor ThemeManager::accentTextColor() { return QColor::fromString(m_isDarkMode ? m_accentTextDark : m_accentTextLight); }

QString ThemeManager::defaultBackColorHex() { return m_isDarkMode ? m_defaultBackDark : m_defaultBackLight; }
QString ThemeManager::activeBackColorHex() { return m_isDarkMode ? m_activeBackDark : m_activeBackLight; }
QString ThemeManager::accentBackColorHex() { return m_isDarkMode ? m_accentBackDark : m_accentBackLight; }
QString ThemeManager::defaultTextColorHex() { return m_isDarkMode ? m_defaultTextDark : m_defaultTextLight; }
QString ThemeManager::accentTextColorHex() { return m_isDarkMode ? m_accentTextDark : m_accentTextLight; }
