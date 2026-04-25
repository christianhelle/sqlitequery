#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QPalette>

struct HighlightingRule {
    QRegularExpression pattern;
    QTextCharFormat format;

    HighlightingRule() = default;
    HighlightingRule(const HighlightingRule&) = default;
    HighlightingRule& operator=(const HighlightingRule&) = default;
    HighlightingRule(HighlightingRule&&) noexcept = default;
    HighlightingRule& operator=(HighlightingRule&&) noexcept = default;
    ~HighlightingRule() = default;
};

class Highlighter final : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit Highlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    QVector<HighlightingRule> highlightingRules;

    static bool isDarkMode();

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // HIGHLIGHTER_H
