# Hockney — History & Learnings

## Project Context

**Project:** SQLite Query Analyzer
**Language:** C++17
**Framework:** Qt6
**Role Focus:** Cross-platform testing, CLI validation, quality assurance

## Testing Environments

```
Windows (MSVC):
  Compiler: Visual Studio 2022 MSVC
  Qt: 6.9.0 MSVC2022
  Build: cmake --build build --config Release
  Deploy: windeployqt.exe ./build/Release/SQLiteQueryAnalyzer.exe

macOS (Clang):
  Compiler: Xcode Clang
  Qt: Homebrew qt@6 or official installer
  Build: cmake --build build --config Release
  Deploy: macdeployqt SQLiteQueryAnalyzer.app

Linux (GCC):
  Compiler: GCC 13.3+
  Qt: apt qt6-base-dev + libxkbcommon-dev
  Build: cmake --build build --config Release --parallel $(nproc)
  Deploy: CPack (DEB, RPM, 7Z, Snap)
```

## CLI Testing

**Critical Command Pattern:**
```bash
QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --help
QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --export-csv /path/to/db.sqlite
QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --run-sql /path/to/script.sql /path/to/db.sqlite
```

The `QT_QPA_PLATFORM=offscreen` flag is REQUIRED for headless CLI testing on Linux.

## Build & Test Workflow

1. **Configure**: `cmake -B build -DCMAKE_BUILD_TYPE=Release`
2. **Build**: `cmake --build build --config Release --parallel 4` (takes ~12s)
3. **Install**: `cmake --install build` (takes <1s)
4. **Test**: Run CLI commands, GUI manual tests

## Key Testing Scenarios

- Export CSV with various data types (integers, text, NULL, special chars)
- Export with large tables (100k+ rows)
- Query execution on read-only databases
- Dark mode toggle
- Window resize with large results
- CLI with non-existent files
- Concurrent operations (export + query simultaneously)

## Cross-Platform Gotchas

- **Windows**: File paths may use backslashes; Qt normalizes them
- **macOS**: App bundle paths differ from CLI build paths
- **Linux**: libxkbcommon-dev required for keyboard input

## Team Coordination

- **Ripley** (Lead) — Reports bugs with full platform/reproduction info
- **Dallas** (Frontend) — Tests UI responsiveness after database ops
- **Fenster** (Backend) — Tests export accuracy, edge cases in data
- **CI/CD** — GitHub Actions automate Windows/Mac/Linux builds; verify green before testing locally

## Learnings & Decisions

(To be filled as sessions progress)
