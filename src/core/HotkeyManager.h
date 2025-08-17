#pragma once

#include <QAbstractNativeEventFilter>
#include <QDateTime>
#include <QObject>
#include <windows.h>
#include "../common/Constants.h"

class HotkeyManager final : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    HotkeyManager(UINT fsModifiers, UINT vk, int id);

private:
    QDateTime lastHotkeyTime;

protected:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

signals:
    void hotkeyPressed(long long id);
};
