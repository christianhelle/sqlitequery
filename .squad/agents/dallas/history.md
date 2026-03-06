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
