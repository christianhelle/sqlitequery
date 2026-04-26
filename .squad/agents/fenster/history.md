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

### 2024-01 - SonarCloud Ownership Resolution

**Context:** Resolved cpp:S5025, cpp:S3230, and cpp:S5350 SonarCloud findings related to memory ownership and const-correctness.

**Key Learnings:**

1. **RAII + Qt Ownership Transfer Pattern**: The solution to cpp:S5025 is `std::make_unique<T>()` followed by `.release()` when transferring to Qt parent containers. This satisfies Sonar's "no raw new" rule while integrating cleanly with Qt's parent-child ownership. Pattern: create with RAII, capture `.get()` if needed for setup, then `.release()` to transfer ownership to Qt.

2. **Qt Parent-Child is Sufficient for Cleanup**: Manual tracking lists like `treeNodes` are redundant and risky. When `QTreeWidgetItem` is added to a tree or parent item via `addTopLevelItem()` or `addChild()`, Qt owns it. Calling `tree->clear()` recursively deletes all children. The separate `qDeleteAll()` pattern created double-deletion risks.

3. **Parent Models to Views for Automatic Cleanup**: Creating `QSqlQueryModel` with a `QTableView` parent (`std::make_unique<QSqlQueryModel>(table)` then `table->setModel(model.release())`) ensures Qt deletes the model when the view is destroyed. The previous unparented model required manual deletion.

4. **In-Class Initializers for Simple Types**: For POD types like `bool loaded = false;` and container types with default constructors like `QList<T> list{};`, in-class initialization is cleaner than constructor initialization lists. This satisfies cpp:S3230 and makes defaults explicit.

5. **Const-Correctness Documents Intent**: When `sender()` is only used to read `objectName()`, declaring it `const auto *senderObject` documents read-only intent and satisfies cpp:S5350.

**Pattern for Qt Ownership Transfer:**
```cpp
// Create with RAII
auto item = std::make_unique<QTreeWidgetItem>();
item->setText(0, "Label");

// Capture raw pointer if needed for later use
const auto itemPtr = item.get();

// Transfer ownership to Qt parent
parent->addChild(item.release());

// Use raw pointer for subsequent operations
itemPtr->doSomething();
```

**Impact:** Eliminated 16 SonarCloud findings (13 cpp:S5025, 3 cpp:S3230, 1 cpp:S5350) without changing runtime behavior. RAII ensures no leaks if exceptions occur before ownership transfer. Reduced risk of double-deletion and dangling pointers.

### 2026-01 - Cross-Team Sonar Sprint Coordination

**Context:** Participated in cross-team effort to resolve 50+ SonarCloud findings across workflows, C++ backend, GUI/docs, and DevOps.

**Your Scope:** 16 findings in database layer (cpp:S5025 ownership, cpp:S3230 initializers, cpp:S5350 const-correctness).

**Team Coordination:**
- Ripley: Workflow security + version constant (5 findings)
- Fenster (you): C++ ownership & const-correctness (16 findings)
- Dallas: GUI move semantics + docs JavaScript (8 findings)
- Hockney: Shell script modernization (21+ findings)

**Status:** All agent work integrated; baseline build passed; no conflicts with other layers.

**Deliverables:** Orchestration logs, consolidated decisions, session log. Ready for Sonar re-scan.
