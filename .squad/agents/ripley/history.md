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

### 2025-01-XX: Sonar Workflow Security and Macro Modernization

**Context:** SonarCloud flagged BLOCKER script injection vulnerabilities (githubactions:S7630) in GitHub Actions workflows and cpp:S5028 macro use in main.cpp.

**Decisions:**
1. **Workflow Input Sanitization:** Moved all `${{ inputs.version }}` direct interpolations into PowerShell `run:` blocks to step-level `env:` variables. PowerShell scripts now reference `$env:VERSION` instead of direct GitHub expression substitution. This prevents script injection attacks if a version string contains malicious content.
2. **Version Constant Migration:** Replaced C preprocessor macro `#define VERSION "1.0.0"` with modern C++ `constexpr auto Version = "1.0.0";` for type safety and debugger visibility.
3. **Workflow Pattern Consistency:** Updated version replacement patterns in both Windows and macOS templates to match the new `constexpr` declaration, ensuring release workflows continue to inject version numbers correctly.

**Impact:**
- Security: Eliminates script injection surface in CI/CD pipelines
- Maintainability: Typed constants provide better IDE support and compile-time safety
- Cross-platform: Pattern works identically across Windows (MSVC) and macOS (Clang) builds

**Gotchas:**
- PowerShell string escaping: Use backtick-escaped quotes for inner strings: `` `"$env:VERSION`" ``
- CMake version update step (line 31 in windows-template.yml) already used `${{ env.VERSION }}` correctly
- The `.pro` file version replacement still uses `VERSION = ...` format (QMake variable assignment)
- GitHub Actions `with:` parameters cannot reference step-level `env:` variables; use job-level `env:` or direct `${{ inputs.* }}` in action parameters
- Artifact paths in upload-artifact actions can use `${{ inputs.version }}` safely (not script injection risk)

### 2026-01 - Cross-Team Sonar Sprint Coordination

**Context:** Led cross-team effort to resolve 50+ SonarCloud findings across workflows, C++ backend, GUI/docs, and DevOps scripts.

**Team Composition:**
- Ripley (you): Workflow security + version constant (5 findings)
- Fenster: C++ ownership & const-correctness (16 findings)
- Dallas: GUI move semantics + docs JavaScript (8 findings)
- Hockney: Shell script modernization (21+ findings)

**Validation:** Baseline build passed; no conflicts between scopes; ready for Sonar re-scan to confirm all 50+ findings resolved.

**Deliverables:** Orchestration logs (`.squad/orchestration-log/2026-04-25T09-21-43Z-*.md`), consolidated decisions (`.squad/decisions.md`), session log (`.squad/log/2026-04-25T09-21-43Z-sonar-cleanup.md`).
