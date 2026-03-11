# Fenster — History & Learnings

## Project Context

**Project:** SQLite Query Analyzer
**Language:** C++17
**Framework:** Qt6 (SQL module, QtConcurrent)
**Role Focus:** Database operations, export, threading, schema introspection

## Key Database Files

```
src/database/
├── database.h/.cpp        # SQLite connection management, threading
├── dbquery.h/.cpp         # Query execution
├── dbanalyzer.h/.cpp      # Schema introspection
├── dbtree.h/.cpp          # Table/column tree widget
├── dbexport.h/.cpp        # Export base class
├── dbexportschema.h/.cpp  # CREATE TABLE export
├── dbexportdata.h/.cpp    # CSV/SQL INSERT export
└── (other database utilities)
```

## Threading & Concurrency Model

1. **Main Thread**: GUI and user interaction (Dallas owns this)
2. **Worker Threads**: Database operations run via QtConcurrent::run()
3. **Thread-Local Storage**: Each worker has its own QSqlDatabase connection
4. **Signals**: Database completion signals back to GUI for result handling

## Export Pipeline

1. **Initiate**: User clicks "Export CSV" (Dallas's button signal)
2. **Spawn**: QtConcurrent::run() on Fenster's export method
3. **Progress**: Export reports progress via signals
4. **Write**: Stream data to CSV files (not all in memory)
5. **Cleanup**: Signal completion, GUI updates status

## Key Patterns

- **Parameterized Queries**: Always use `addBindValue()` for SQL params
- **Transactions**: Wrap multi-step operations for consistency
- **Foreign Keys**: Enable with PRAGMA; respect referential integrity
- **Streaming**: Don't load entire dataset into memory; write incrementally

## Cross-Platform Database Notes

- **Locking**: Always use transactions to ensure consistency across Windows/Mac/Linux
- **Case Sensitivity**: Column names are case-insensitive in SQLite but case-preserving; be consistent
- **File Paths**: Use Qt's QFile/QFileInfo for cross-platform path handling

## Team Coordination

- **Dallas** (Frontend) — Calls your database slots from GUI signals
- **Hockney** (Tester) — Tests export behavior on all platforms
- **Ripley** (Lead) — Architecture for major database refactors

## Learnings & Decisions

(To be filled as sessions progress)
