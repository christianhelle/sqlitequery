# Ripley — History & Learnings

## Project Context

**Project:** SQLite Query Analyzer
**Language:** C++17
**Framework:** Qt6 (Core, Gui, Sql, Widgets, Concurrent)
**Platforms:** Windows (MSVC), macOS (Clang), Linux (GCC)
**Build System:** CMake 3.16+
**Code Quality:** SonarCloud, clang-format (LLVM style)

## Key Files & Structure

```
src/
├── main.cpp               # CLI/GUI entry point, argument routing
├── gui/
│   ├── mainwindow.cpp/.h  # Main application window, signal/slot coordination
│   ├── mainwindow.ui      # Qt Designer UI layout
│   ├── highlighter.cpp    # SQL syntax highlighting
├── database/
│   ├── database.cpp       # SQLite connection management, threading
│   ├── dbquery.cpp        # Query execution
│   ├── dbtree.cpp         # Table/column tree view
│   ├── dbexport*.cpp      # Export orchestration (CSV, SQL, schema)
├── cli/
│   ├── export.cpp         # CLI CSV export handler
│   └── script.cpp         # CLI SQL script execution
├── settings/
│   ├── settings.cpp       # Application state persistence
│   └── recentfiles.cpp    # Recent file tracking
└── threading/
    ├── cancellation.cpp   # Cancellation token support
    └── mainthread.h       # Thread safety helpers
```

## Architectural Patterns

1. **Signal/Slot**: Core event communication. GUI signals trigger database operations asynchronously.
2. **Threading**: QtConcurrent for async work. Each QSqlDatabase connection is per-thread.
3. **Export Pipeline**: dbexport base class → dbexportschema, dbexportdata (CSV, SQL INSERT)
4. **Settings Persistence**: Qt QSettings-based, auto-loaded on startup
5. **Progress Reporting**: Abstract progress interface, implemented by GUI and CLI

## Team Context

- **Dallas** — Handles all GUI changes: mainwindow.ui, signals, UI flow, dark mode, styling
- **Fenster** — Backend database changes: queries, export, schema operations, threading
- **Hockney** — Validation: cross-platform testing, CLI verification, edge case detection

## Learnings & Decisions

(To be filled as sessions progress)
