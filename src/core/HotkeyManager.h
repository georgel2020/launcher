#pragma once

#include <QAbstractNativeEventFilter>
#include <QDateTime>
#include <QObject>
#include <windows.h>

class HotkeyManager final : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    static HotkeyManager *instance();

    static bool registerHotkey(UINT fsModifiers, UINT vk, int id);

    HotkeyManager(const HotkeyManager &) = delete;
    HotkeyManager &operator=(const HotkeyManager &) = delete;

signals:
    void hotkeyPressed(long long id);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    explicit HotkeyManager(QObject *parent = nullptr);
};
