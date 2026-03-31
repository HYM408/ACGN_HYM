#ifndef GLOBAL_HOTKEY_MANAGER_H
#define GLOBAL_HOTKEY_MANAGER_H

#include <QHash>
#include <QObject>
#include <windows.h>

class GlobalHotkeyManager : public QObject
{
    Q_OBJECT

public:
    explicit GlobalHotkeyManager(const QWidget *window, QObject *parent = nullptr);
    bool registerHotKey(UINT id, UINT modifiers, UINT vk, const std::function<void()> &callback);
    void unregisterHotKey(UINT id);
    bool handleHotKey(UINT id);

private:
    HWND m_hwnd = nullptr;
    QHash<UINT, std::function<void()>> callbacks;
};

#endif // GLOBAL_HOTKEY_MANAGER_H