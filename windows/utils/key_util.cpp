#include "key_util.h"
#include <QMap>
#include <windows.h>
#include <QKeySequence>

static QMap<int, QString> map = {
    {VK_ESCAPE, "Esc"}, {VK_OEM_3, "`"}, {VK_TAB, "Tab"}, {VK_CAPITAL, "CapsLock"},
    {VK_OEM_MINUS,"-"}, {VK_OEM_PLUS, "+"},
    {VK_OEM_4, "["}, {VK_OEM_6, "]"},
    {VK_OEM_1, ";"}, {VK_OEM_7, "'"}, {VK_OEM_5, "\\"},
    {VK_OEM_COMMA, ","}, {VK_OEM_PERIOD, "."}, {VK_OEM_2, "/"},
    {VK_LSHIFT, "左Shift"}, {VK_RSHIFT, "右Shift"},
    {VK_LCONTROL, "左Control"}, {VK_RCONTROL, "右Control"},
    {VK_LMENU, "左Alt"}, {VK_RMENU, "右Alt"},
    {VK_LWIN, "左Win"}, {VK_RWIN, "右Win"},
    {VK_SPACE, "Space"},
    {VK_BACK, "Backspace"}, {VK_RETURN, "Return"},
    {VK_INSERT, "Insert"}, {VK_DELETE, "Delete"}, {VK_HOME, "Home"}, {VK_END, "End"}, {VK_PRIOR, "PageUp"}, {VK_NEXT, "PageDown"},
    {VK_UP, "Up"}, {VK_DOWN, "Down"}, {VK_LEFT, "Left"}, {VK_RIGHT, "Right"},
    {VK_F1, "F1"}, {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"}, {VK_F5, "F5"}, {VK_F6, "F6"}, {VK_F7, "F7"}, {VK_F8, "F8"}, {VK_F9, "F9"}, {VK_F10, "F10"}, {VK_F11, "F11"}, {VK_F12, "F12"}
};

static int nameToVirtualKey(const QString &name)
{   // 键名->虚拟键码
    for (auto it = map.begin(); it != map.end(); ++it) if (it.value().compare(name) == 0) return it.key();
    const QChar ch = name[0];
    if (ch.isLetterOrNumber()) {
        const int vk = ch.unicode();
        if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) return vk;
    }
    return 27;
}

QKeySequence virtualKeyToKeySequence(const int vk)
{   // 虚拟键码->QKeySequence
    QString name;
    if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) name = {QChar(vk)};
    else name = map.value(vk, QString());
    if (name.isEmpty()) return {};
    return QKeySequence::fromString(name);
}

int keySequenceToVirtualKey(const QKeySequence &ks)
{   // QKeySequence->虚拟键码
    if (ks.isEmpty()) return 27;
    const QString str = ks.toString();
    return nameToVirtualKey(str);
}