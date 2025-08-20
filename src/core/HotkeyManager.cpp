#include "HotkeyManager.h"
#include <QApplication>

HotkeyManager *HotkeyManager::instance()
{
    static HotkeyManager singleInstance;
    return &singleInstance;
}

HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent) { qApp->installNativeEventFilter(this); }

/**
 * Register a global hotkey with Windows API.
 *
 * @param fsModifiers The modifier keys that must be pressed.
 * @param vk The virtual key code of the key to be registered.
 * @param id A unique integer identifier for this hotkey.
 * @return True if the hotkey is successfully registered; false otherwise.
 */
bool HotkeyManager::registerHotkey(const UINT fsModifiers, const UINT vk, const int id) { return RegisterHotKey(nullptr, id, MOD_NOREPEAT | fsModifiers, vk); }

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
        if (const MSG *pMsg = static_cast<MSG *>(message); pMsg->message == WM_HOTKEY)
            emit hotkeyPressed(static_cast<long long>(pMsg->wParam));

    return false;
}
