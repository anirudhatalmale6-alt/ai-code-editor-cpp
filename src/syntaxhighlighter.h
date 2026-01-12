#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_highlightingRules;

    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_preprocessorFormat;
    QTextCharFormat m_singleLineCommentFormat;
    QTextCharFormat m_multiLineCommentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_charFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_operatorFormat;
};

#endif // SYNTAXHIGHLIGHTER_H
