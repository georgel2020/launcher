#pragma once

#include <QAbstractNativeEventFilter>
#include <QDateTime>
#include <QObject>
#include <windows.h>

class HotkeyManager final : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit HotkeyManager(UINT fsModifiers, UINT vk, int id, QObject* parent = nullptr);

private:
    QDateTime lastHotkeyTime;

protected:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

signals:
    void hotkeyPressed(long long id);
};
