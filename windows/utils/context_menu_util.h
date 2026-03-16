#ifndef CONTEXT_MENU_UTIL_H
#define CONTEXT_MENU_UTIL_H

#include <QWidget>

class QTextEdit;
class QLineEdit;

enum ContextMenuOption {
    CMO_Cut = 0x1,
    CMO_Copy = 0x2,
    CMO_Paste = 0x4,
    CMO_SelectAll = 0x8,
    CMO_Default = CMO_Cut | CMO_Copy | CMO_Paste | CMO_SelectAll
};

Q_DECLARE_FLAGS(ContextMenuOptions, ContextMenuOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(ContextMenuOptions)

class LineEditContextMenu : public QWidget
{
    Q_OBJECT

public:
    explicit LineEditContextMenu(QLineEdit *lineEdit, const QPoint &globalPos, ContextMenuOptions options = CMO_Default);
};

class TextEditContextMenu : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditContextMenu(QTextEdit *textEdit, const QPoint &globalPos, ContextMenuOptions options = CMO_Default);
};

void setupLineEditCustomContextMenu(QLineEdit *lineEdit, ContextMenuOptions options = CMO_Default);
void setupTextEditCustomContextMenu(QTextEdit *textEdit, ContextMenuOptions options = CMO_Default);

#endif // CONTEXT_MENU_UTIL_H