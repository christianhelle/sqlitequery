#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(isDarkMode() ? Qt::cyan : Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns;
    keywordPatterns << "\\bselect\\b" << "\\bfrom\\b" << "\\blimit\\b"  << "\\bwhere\\b" << "\\binsert\\b"
                    << "\\bdelete\\b" << "\\bupdate\\b" << "\\binto\\b" << "\\bvalues\\b"
                    << "\\bleft\\b" << "\\inner\\b" << "\\bjoin\\b" << "\\bright\\b"
                    << "\\bouter\\b" << "\\bunion\\b" << "\\ball\\b" << "\\bhaving\\b"
                    << "\\border\\b" << "\\bby\\b" << "\\basc\\b" << "\\bdesc\\b"
                    << "\\bhaving\\b" << "\\bin\\b" << "\\bcreate\\b" << "\\bdrop\\b" << "\\btable\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(isDarkMode() ? Qt::white : Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(isDarkMode() ? Qt::green :Qt::red);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(isDarkMode() ? Qt::green : Qt::darkGreen);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(isDarkMode() ? Qt::white : Qt::blue);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

bool Highlighter::isDarkMode()
{
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

