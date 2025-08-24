#include "ThemeManager.h"
#include <QApplication>
#include <QStyleHints>
#include "../common/Constants.h"

void ThemeManager::initTheme() { m_isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark; }

QColor ThemeManager::defaultBackColor() { return QColor::fromString(m_isDarkMode ? DEFAULT_BACK_DARK : DEFAULT_BACK_LIGHT); }
QColor ThemeManager::activeBackColor() { return QColor::fromString(m_isDarkMode ? ACTIVE_BACK_DARK : ACTIVE_BACK_LIGHT); }
QColor ThemeManager::accentBackColor() { return QColor::fromString(m_isDarkMode ? ACCENT_BACK_DARK : ACCENT_BACK_LIGHT); }
QColor ThemeManager::defaultTextColor() { return QColor::fromString(m_isDarkMode ? DEFAULT_TEXT_DARK : DEFAULT_TEXT_LIGHT); }
QColor ThemeManager::accentTextColor() { return QColor::fromString(m_isDarkMode ? ACCENT_TEXT_DARK : ACCENT_TEXT_LIGHT); }

QString ThemeManager::defaultBackColorHex() { return m_isDarkMode ? DEFAULT_BACK_DARK : DEFAULT_BACK_LIGHT; }
QString ThemeManager::activeBackColorHex() { return m_isDarkMode ? ACTIVE_BACK_DARK : ACTIVE_BACK_LIGHT; }
QString ThemeManager::accentBackColorHex() { return m_isDarkMode ? ACCENT_BACK_DARK : ACCENT_BACK_LIGHT; }
QString ThemeManager::defaultTextColorHex() { return m_isDarkMode ? DEFAULT_TEXT_DARK : DEFAULT_TEXT_LIGHT; }
QString ThemeManager::accentTextColorHex() { return m_isDarkMode ? ACCENT_TEXT_DARK : ACCENT_TEXT_LIGHT; }
