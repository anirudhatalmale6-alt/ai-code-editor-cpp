#include "codeeditor.h"
#include "syntaxhighlighter.h"
#include <QPainter>
#include <QTextBlock>
#include <QKeyEvent>
#include <QScrollBar>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);
    m_highlighter = new SyntaxHighlighter(document());

    // Set font
    QFont font("Consolas", 11);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    setFont(font);

    // Set tab width
    QFontMetrics metrics(font);
    setTabStopDistance(4 * metrics.horizontalAdvance(' '));

    // Editor settings
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);

    // Connect signals
    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // Placeholder text
    setPlaceholderText("// Enter your C/C++ code here...\n\n#include <stdio.h>\n\nint main() {\n    printf(\"Hello, World!\\n\");\n    return 0;\n}");
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 15 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                         lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(40, 40, 40);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(30, 30, 30));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(100, 100, 100));

            if (textCursor().blockNumber() == blockNumber) {
                painter.setPen(QColor(200, 200, 200));
            }

            painter.drawText(0, top, m_lineNumberArea->width() - 8,
                           fontMetrics().height(),
                           Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    // Auto-indentation
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QString currentLine = cursor.block().text();

        // Get leading whitespace
        QString indent;
        for (QChar c : currentLine) {
            if (c == ' ' || c == '\t') {
                indent += c;
            } else {
                break;
            }
        }

        // Check if we need to add extra indent (after { )
        QString trimmed = currentLine.trimmed();
        if (trimmed.endsWith('{')) {
            indent += "    ";
        }

        // Insert newline with indentation
        cursor.insertText("\n" + indent);
        setTextCursor(cursor);
        return;
    }

    // Auto-close brackets
    if (event->key() == Qt::Key_BraceLeft) {
        QTextCursor cursor = textCursor();
        cursor.insertText("{}");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    if (event->key() == Qt::Key_ParenLeft) {
        QTextCursor cursor = textCursor();
        cursor.insertText("()");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    if (event->key() == Qt::Key_BracketLeft) {
        QTextCursor cursor = textCursor();
        cursor.insertText("[]");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    if (event->key() == Qt::Key_QuoteDbl) {
        QTextCursor cursor = textCursor();
        if (!cursor.hasSelection()) {
            cursor.insertText("\"\"");
            cursor.movePosition(QTextCursor::Left);
            setTextCursor(cursor);
            return;
        }
    }

    // Tab to spaces
    if (event->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        cursor.insertText("    ");
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}
