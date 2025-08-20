#include "ThemeManager.h"
#include <QApplication>
#include <QStyleHints>
#include "../common/Constants.h"



void ThemeManager::initTheme() { m_isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark; }

QColor ThemeManager::primaryBackColor() { return QColor::fromString(m_isDarkMode ? PRIMARY_BACK_DARK : PRIMARY_BACK_LIGHT); }
QColor ThemeManager::secondaryBackColor() { return QColor::fromString(m_isDarkMode ? SECONDARY_BACK_DARK : SECONDARY_BACK_LIGHT); }
QColor ThemeManager::primaryTextColor() { return QColor::fromString(m_isDarkMode ? PRIMARY_TEXT_DARK : PRIMARY_TEXT_LIGHT); }
QColor ThemeManager::secondaryTextColor() { return QColor::fromString(m_isDarkMode ? SECONDARY_TEXT_DARK : SECONDARY_TEXT_LIGHT); }

QString ThemeManager::primaryBackColorHex() { return m_isDarkMode ? PRIMARY_BACK_DARK : PRIMARY_BACK_LIGHT; }
QString ThemeManager::secondaryBackColorHex() { return m_isDarkMode ? SECONDARY_BACK_DARK : SECONDARY_BACK_LIGHT; }
QString ThemeManager::primaryTextColorHex() { return m_isDarkMode ? PRIMARY_TEXT_DARK : PRIMARY_TEXT_LIGHT; }
QString ThemeManager::secondaryTextColorHex() { return m_isDarkMode ? SECONDARY_TEXT_DARK : SECONDARY_TEXT_LIGHT; }
