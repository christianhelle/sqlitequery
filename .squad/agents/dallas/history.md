# Dallas — History & Learnings

## Project Context

**Project:** SQLite Query Analyzer
**Language:** C++17
**Framework:** Qt6 (Core, Gui, Sql, Widgets, Concurrent)
**Role Focus:** Qt GUI, signals/slots, UI layouts, dark mode, styling

## Key GUI Files

```
src/gui/
├── mainwindow.h/.cpp      # Main application window, event handling
├── mainwindow.ui          # Qt Designer layout file (edit with Qt Designer)
├── highlighter.h/.cpp     # SQL syntax highlighting
├── prompts.h/.cpp         # Dialog prompts
└── (other UI components)
```

## Qt Fundamentals for This Project

1. **Signals & Slots**: mainwindow emits signals when user interacts; slots call Fenster's database methods
2. **UI Files**: Edit .ui with Qt Designer, rebuild to generate ui_mainwindow.h
3. **Threading**: Never block GUI thread. Use QtConcurrent::run() for database calls
4. **Dark Mode**: Qt6 handles theme switching via platform palette — no manual code needed
5. **Memory**: Qt owns UI objects. Use parent relationships, never manual delete

## Team Coordination

- **Fenster** (Backend) — Provide database operation slots that GUI signals can call
- **Hockney** (Tester) — Validate UI on Windows, macOS, Linux after changes
- **Ripley** (Lead) — Architecture for major UI refactors

## Learnings & Decisions

(To be filled as sessions progress)

## Learnings

### SonarCloud C++ noexcept Compliance (2025-01)

Fixed cpp:S5018 in `HighlightingRule` struct by explicitly defaulting all special members and marking move operations `noexcept`. Qt6's `QRegularExpression` and `QTextCharFormat` both support noexcept moves, making the defaulted move operations safe. The explicit declarations satisfy SonarCloud's requirement for predictable move semantics in container-stored types.

### JavaScript Modern Best Practices (2025-01)

Resolved JavaScript maintainability issues in `docs/script.js`:
- Replaced `setAttribute/getAttribute('data-theme')` with `dataset.theme` (S7761) - cleaner DOM API
- Used proper optional chaining for MediaQueryList: stored `globalThis.matchMedia?.()` result and chained access to `.matches` and `.addEventListener()` (S6582, S7764) - prevents property access on undefined
- Migrated from `window` to `globalThis` for better cross-environment compatibility
- Theme toggle behavior remains unchanged: saved theme preference, default light mode, system dark detection, and live system theme change listener all preserved

### 2026-01 - Cross-Team Sonar Sprint Coordination

**Context:** Contributed to cross-team effort resolving 50+ SonarCloud findings.

**Your Scope:** 8 findings in GUI and documentation (cpp:S5018 noexcept move semantics, JavaScript S7761/S6582/S7764 modernization).

**Team Coordination:**
- Ripley: Workflow security + version constant (5 findings)
- Fenster: C++ ownership & const-correctness (16 findings)
- Dallas (you): GUI move semantics + docs JavaScript (8 findings)
- Hockney: Shell script modernization (21+ findings)

**Status:** All changes integrated; baseline build validated; zero conflicts with backend/database layer.

**Next:** Sonar re-scan to confirm 50+ findings resolved across all domains.
