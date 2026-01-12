#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Keywords
    m_keywordFormat.setForeground(QColor(86, 156, 214));  // VS Code blue
    m_keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        "\\bauto\\b", "\\bbreak\\b", "\\bcase\\b", "\\bcatch\\b",
        "\\bclass\\b", "\\bconst\\b", "\\bconstexpr\\b", "\\bcontinue\\b",
        "\\bdefault\\b", "\\bdelete\\b", "\\bdo\\b", "\\belse\\b",
        "\\benum\\b", "\\bexplicit\\b", "\\bexport\\b", "\\bextern\\b",
        "\\bfalse\\b", "\\bfor\\b", "\\bfriend\\b", "\\bgoto\\b",
        "\\bif\\b", "\\binline\\b", "\\bmutable\\b", "\\bnamespace\\b",
        "\\bnew\\b", "\\bnoexcept\\b", "\\bnullptr\\b", "\\boperator\\b",
        "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\bregister\\b",
        "\\breinterpret_cast\\b", "\\breturn\\b", "\\bsizeof\\b", "\\bstatic\\b",
        "\\bstatic_assert\\b", "\\bstatic_cast\\b", "\\bstruct\\b", "\\bswitch\\b",
        "\\btemplate\\b", "\\bthis\\b", "\\bthrow\\b", "\\btrue\\b",
        "\\btry\\b", "\\btypedef\\b", "\\btypeid\\b", "\\btypename\\b",
        "\\bunion\\b", "\\busing\\b", "\\bvirtual\\b", "\\bvolatile\\b",
        "\\bwhile\\b", "\\boverride\\b", "\\bfinal\\b", "\\balignof\\b",
        "\\balignos\\b", "\\basm\\b", "\\bdecltype\\b", "\\bthread_local\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    // Types
    m_typeFormat.setForeground(QColor(78, 201, 176));  // VS Code teal
    const QString typePatterns[] = {
        "\\bbool\\b", "\\bchar\\b", "\\bchar16_t\\b", "\\bchar32_t\\b",
        "\\bdouble\\b", "\\bfloat\\b", "\\bint\\b", "\\blong\\b",
        "\\bshort\\b", "\\bsigned\\b", "\\bunsigned\\b", "\\bvoid\\b",
        "\\bwchar_t\\b", "\\bint8_t\\b", "\\bint16_t\\b", "\\bint32_t\\b",
        "\\bint64_t\\b", "\\buint8_t\\b", "\\buint16_t\\b", "\\buint32_t\\b",
        "\\buint64_t\\b", "\\bsize_t\\b", "\\bptrdiff_t\\b", "\\bintptr_t\\b",
        "\\buintptr_t\\b", "\\bstring\\b", "\\bvector\\b", "\\bmap\\b",
        "\\bset\\b", "\\blist\\b", "\\bqueue\\b", "\\bstack\\b",
        "\\barray\\b", "\\bunordered_map\\b", "\\bunordered_set\\b",
        "\\bshared_ptr\\b", "\\bunique_ptr\\b", "\\bweak_ptr\\b",
        "\\bFILE\\b"
    };
    for (const QString &pattern : typePatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_typeFormat;
        m_highlightingRules.append(rule);
    }

    // Preprocessor directives
    m_preprocessorFormat.setForeground(QColor(155, 155, 155));  // Gray
    rule.pattern = QRegularExpression("^\\s*#.*");
    rule.format = m_preprocessorFormat;
    m_highlightingRules.append(rule);

    // Numbers
    m_numberFormat.setForeground(QColor(181, 206, 168));  // VS Code light green
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*([eE][+-]?[0-9]+)?[fFlLuU]*\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);

    // Hex numbers
    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+[uUlL]*\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);

    // Binary numbers
    rule.pattern = QRegularExpression("\\b0[bB][01]+\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);

    // Functions
    m_functionFormat.setForeground(QColor(220, 220, 170));  // VS Code yellow
    rule.pattern = QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);

    // Strings
    m_stringFormat.setForeground(QColor(206, 145, 120));  // VS Code orange
    rule.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"");
    rule.format = m_stringFormat;
    m_highlightingRules.append(rule);

    // Character literals
    m_charFormat.setForeground(QColor(206, 145, 120));  // VS Code orange
    rule.pattern = QRegularExpression("'(?:[^'\\\\]|\\\\.)*'");
    rule.format = m_charFormat;
    m_highlightingRules.append(rule);

    // Single-line comments
    m_singleLineCommentFormat.setForeground(QColor(106, 153, 85));  // VS Code green
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    // Multi-line comments
    m_multiLineCommentFormat.setForeground(QColor(106, 153, 85));  // VS Code green
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply highlighting rules
    for (const HighlightingRule &rule : qAsConst(m_highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(m_commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match = m_commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = text.indexOf(m_commentStartExpression, startIndex + commentLength);
    }
}
