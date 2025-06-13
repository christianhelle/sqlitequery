#include "highlighter.h"
#include <QGuiApplication>
#include <QStyleHints>
#include <QPalette>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    keywordFormat.setForeground(isDarkMode() ? Qt::cyan : Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns;
    keywordPatterns << "\\bselect\\b" << "\\bfrom\\b" << "\\blimit\\b" << "\\bwhere\\b" << "\\binsert\\b"
            << "\\bdelete\\b" << "\\bupdate\\b" << "\\binto\\b" << "\\bvalues\\b"
            << "\\bleft\\b" << "\\inner\\b" << "\\bjoin\\b" << "\\bright\\b"
            << "\\bouter\\b" << "\\bunion\\b" << "\\ball\\b" << "\\bhaving\\b"
            << "\\border\\b" << "\\bby\\b" << "\\basc\\b" << "\\bdesc\\b"
            << "\\bhaving\\b" << "\\bin\\b" << "\\bcreate\\b" << "\\bdrop\\b" << "\\btable\\b";

    foreach(const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(isDarkMode() ? Qt::white : Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(isDarkMode() ? Qt::green : Qt::red);
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

bool Highlighter::isDarkMode() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Qt 6.5+ has colorScheme() method
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
#else
    // Fallback for older Qt versions: check palette colors
    const QPalette palette = QGuiApplication::palette();
    const QColor windowColor = palette.color(QPalette::Window);
    const QColor textColor = palette.color(QPalette::WindowText);
    
    // Simple heuristic: if window is darker than text, it's likely dark mode
    return windowColor.lightness() < textColor.lightness();
#endif
}

void Highlighter::highlightBlock(const QString &text) {
    for (const auto &[pattern, format]: std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(static_cast<int>(match.capturedStart()),
                      static_cast<int>(match.capturedLength()),
                      format);
        }
    }
    setCurrentBlockState(0);

    qsizetype startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        const qsizetype endIndex = match.capturedStart();
        qsizetype commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(static_cast<int>(startIndex), static_cast<int>(commentLength), multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
