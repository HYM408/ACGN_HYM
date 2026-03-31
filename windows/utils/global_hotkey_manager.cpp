#include "global_hotkey_manager.h"
#include <QWidget>

GlobalHotkeyManager::GlobalHotkeyManager(const QWidget *window, QObject *parent) : QObject(parent)
{
    if (window->winId()) m_hwnd = reinterpret_cast<HWND>(window->winId());
}

bool GlobalHotkeyManager::registerHotKey(const UINT id, const UINT modifiers, const UINT vk, const std::function<void()> &callback)
{   // 注册快捷键
    if (callbacks.contains(id)) return false;
    if (!RegisterHotKey(m_hwnd, static_cast<int>(id), modifiers, vk)) return false;
    callbacks.insert(id, callback);
    return true;
}

void GlobalHotkeyManager::unregisterHotKey(const UINT id)
{   // 注销快捷键
    if (!callbacks.contains(id)) return;
    UnregisterHotKey(m_hwnd, static_cast<int>(id));
    callbacks.remove(id);
}

bool GlobalHotkeyManager::handleHotKey(const UINT id)
{   // 处理快捷键操作
    const auto it = callbacks.find(id);
    if (it != callbacks.end()) {
        it.value()();
        return true;
    }
    return false;
}