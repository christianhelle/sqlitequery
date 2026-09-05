#ifndef SQLIDENTIFIER_H
#define SQLIDENTIFIER_H

#include <QString>

// The name of a Table or Column as written into SQL. A name may hold spaces, a
// reserved word, or a double quote, and only the delimited form survives all
// three -- so identifiers are delimited here and never interpolated raw.
//
// The delimiters come back with the name, rather than being left for the caller
// to add, because a caller that remembers the escaping but forgets the quotes
// still produces SQL that looks plausible and runs wrong.
//
// Total by design: an empty name yields an empty delimited identifier, which
// SQLite rejects. An identifier arriving empty means the caller's own lookup
// found nothing, which is a different bug from a quoting one -- guard the
// lookup, not the result of this.
inline QString quotedIdentifier(const QString &name) {
    // Explicit Qt character types rather than char literals: Qt 6.9 turned
    // off the implicit QChar conversions these used to rely on.
    QString escaped = name;
    // A quoted identifier escapes a double quote by doubling it.
    escaped.replace(QLatin1Char('"'), QStringLiteral("\"\""));
    return QLatin1Char('"') + escaped + QLatin1Char('"');
}

#endif // SQLIDENTIFIER_H
