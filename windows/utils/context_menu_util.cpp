#include "context_menu_util.h"
#include <QScreen>
#include <QLineEdit>
#include <QTextEdit>
#include <QClipboard>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include "../config.h"

LineEditContextMenu::LineEditContextMenu(QLineEdit* lineEdit, const QPoint& globalPos, ContextMenuOptions options) : QWidget(nullptr)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedWidth(100);
    const QString selectedText = lineEdit->selectedText();
    const QString fullText = lineEdit->text();
    const int selStart = lineEdit->selectionStart();
    const int selLength = selectedText.length();
    const int cursorPos = lineEdit->cursorPosition();
    const bool readOnly = lineEdit->isReadOnly();
    const QColor color2 = getColor("color2", QColor("#f2ecf4"));
    const QColor color3 = getColor("color3", QColor("#e1dbe4"));
    auto* container = new QFrame(this);
    container->setStyleSheet(QString("QFrame {background-color: %1; border-radius: 4px}").arg(color2.name()));
    auto* shadow = new QGraphicsDropShadowEffect(container);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 2);
    container->setGraphicsEffect(shadow);
    auto* layout = new QVBoxLayout(container);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    auto createButton = [color3](const QString& text, const bool enabled) {
        auto* btn = new QPushButton(text);
        btn->setStyleSheet(QString("QPushButton {border: none; padding: 10px 12px; font-size: 13px}"
                                   "QPushButton:hover {background-color: %1}"
                                   "QPushButton:disabled {color: #aaa}").arg(color3.name()));
        btn->setEnabled(enabled);
        return btn;
    };
    if (options & CMO_Cut) {
        auto* cutBtn = createButton("剪切", !selectedText.isEmpty() && !readOnly);
        connect(cutBtn, &QPushButton::clicked, this, [=] {
            close();
            QApplication::clipboard()->setText(selectedText);
            if (selStart >= 0) {
                QString newText = fullText;
                newText.remove(selStart, selLength);
                lineEdit->setText(newText);
                lineEdit->setCursorPosition(selStart);
            }
        });
        layout->addWidget(cutBtn);
    }
    if (options & CMO_Copy) {
        auto* copyBtn = createButton("复制", !selectedText.isEmpty());
        connect(copyBtn, &QPushButton::clicked, this, [=] {
            close();
            QApplication::clipboard()->setText(selectedText);
        });
        layout->addWidget(copyBtn);
    }
    if (options & CMO_Paste) {
        auto* pasteBtn = createButton("粘贴", !readOnly && !QApplication::clipboard()->text().isEmpty());
        connect(pasteBtn, &QPushButton::clicked, this, [=] {
            close();
            const QString clipText = QApplication::clipboard()->text();
            if (clipText.isEmpty()) return;
            QString newText = fullText;
            int insertPos = cursorPos;
            if (selStart >= 0) {
                newText.remove(selStart, selLength);
                insertPos = selStart;
            }
            newText.insert(insertPos, clipText);
            lineEdit->setText(newText);
            lineEdit->setCursorPosition(insertPos + clipText.length());
        });
        layout->addWidget(pasteBtn);
    }
    if (options & CMO_SelectAll) {
        auto* selectAllBtn = createButton("全选", !fullText.isEmpty());
        connect(selectAllBtn, &QPushButton::clicked, this, [=] {
            close();
            lineEdit->selectAll();
        });
        layout->addWidget(selectAllBtn);
    }
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(container);
    move(globalPos - QPoint(10, 10));
}

TextEditContextMenu::TextEditContextMenu(QTextEdit* textEdit, const QPoint& globalPos, ContextMenuOptions options) : QWidget(nullptr)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedWidth(100);
    QTextCursor cursor = textEdit->textCursor();
    const QString selectedText = cursor.selectedText();
    const QString fullText = textEdit->toPlainText();
    const QColor color2 = getColor("color2", QColor("#f2ecf4"));
    const QColor color3 = getColor("color3", QColor("#e1dbe4"));
    auto* container = new QFrame(this);
    container->setStyleSheet(QString("QFrame {background-color: %1; border-radius: 4px}").arg(color2.name()));
    auto* shadow = new QGraphicsDropShadowEffect(container);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 2);
    container->setGraphicsEffect(shadow);
    auto* layout = new QVBoxLayout(container);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    auto createButton = [color3](const QString& text, const bool enabled) {
        auto* btn = new QPushButton(text);
        btn->setStyleSheet(QString("QPushButton {border: none; padding: 10px 12px; font-size: 13px}"
                                   "QPushButton:hover {background-color: %1}"
                                   "QPushButton:disabled {color: #aaa}").arg(color3.name()));
        btn->setEnabled(enabled);
        return btn;
    };
    if (options & CMO_Copy) {
        auto* copyBtn = createButton("复制", !selectedText.isEmpty());
        connect(copyBtn, &QPushButton::clicked, this, [=] {
            close();
            QApplication::clipboard()->setText(selectedText);
        });
        layout->addWidget(copyBtn);
    }
    if (options & CMO_SelectAll) {
        auto* selectAllBtn = createButton("全选", !fullText.isEmpty());
        connect(selectAllBtn, &QPushButton::clicked, this, [=] {
            close();
            textEdit->selectAll();
        });
        layout->addWidget(selectAllBtn);
    }
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(container);
    move(globalPos - QPoint(10, 10));
}

void setupLineEditCustomContextMenu(QLineEdit* lineEdit, ContextMenuOptions options)
{   // QLineEdit 封装
    lineEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(lineEdit, &QLineEdit::customContextMenuRequested, lineEdit, [lineEdit, options](const QPoint& pos) {
        (new LineEditContextMenu(lineEdit, lineEdit->mapToGlobal(pos), options))->show();
    });
}

void setupTextEditCustomContextMenu(QTextEdit* textEdit, ContextMenuOptions options)
{   // QTextEdit 封装
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(textEdit, &QTextEdit::customContextMenuRequested, textEdit, [textEdit, options](const QPoint& pos) {
        (new TextEditContextMenu(textEdit, textEdit->mapToGlobal(pos), options))->show();
    });
}