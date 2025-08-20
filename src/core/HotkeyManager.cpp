#include "HotkeyManager.h"
#include <QApplication>
#include "../common/Constants.h"

/**
 * Register a global hotkey in Windows.
 *
 * @param parent The parent QObject.
 * @param fsModifiers The modifiers.
 * @param vk The virtual key.
 * @param id The unique id of the hotkey.
 */
HotkeyManager::HotkeyManager(const UINT fsModifiers, const UINT vk, const int id, QObject *parent) : QObject(parent)
{
    RegisterHotKey(nullptr, id, MOD_NOREPEAT | fsModifiers, vk);
    lastHotkeyTime = QDateTime::currentDateTime();
    qApp->installNativeEventFilter(this);
}

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        const MSG *pMsg = static_cast<MSG *>(message);
        if (pMsg->message == WM_HOTKEY && lastHotkeyTime.msecsTo(QDateTime::currentDateTime()) > MIN_HOTKEY_TIME_INTERVAL)
        {
            lastHotkeyTime = QDateTime::currentDateTime();

            emit hotkeyPressed(static_cast<long long>(pMsg->wParam));
        }
    }
    return false;
}
