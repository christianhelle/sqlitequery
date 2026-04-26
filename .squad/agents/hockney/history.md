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

## Learnings

### Sonar Shell Script Cleanup (Jan 2025)

Successfully resolved 21+ Sonar findings in installer and build scripts:

- **install.sh**: Added default branches to architecture/OS case statements, replaced `[ ... ]` with `[[ ... ]]` for modern Bash conditional tests, added explicit `return 0` statements to all functions, assigned positional parameters to local variables before use, and extracted repeated GitHub URL regex patterns.
- **src/project/build.sh**: Modernized conditional tests with `[[ ... ]]` and hardened `DISABLE_SNAP` variable access with `${DISABLE_SNAP:-}` to survive `set -u` mode.

All changes preserve existing behavior:
- Functions that output to stdout continue to do so
- Error exits remain unchanged
- Script argument parsing logic preserved
- Environment variable behavior unchanged

Key insights:
- Shell script modernization for code quality scanners must balance static analysis rules with runtime behavior preservation, especially for installer scripts that may run in varied shell environments.
- With `set -u` mode, positional parameter access like `$2` must come AFTER checking argument count with `$#` to avoid "unbound variable" errors before the intended error handling can execute.
- Both scripts validated successfully with `bash -n` using Git Bash on Windows.

### 2026-01 - Cross-Team Sonar Sprint Coordination

**Context:** Contributed shell script improvements to cross-team Sonar cleanup sprint targeting 50+ findings.

**Your Scope:** 21+ findings in installation and build scripts (shell modernization: default cases, conditionals, returns, variables, literals).

**Team Coordination:**
- Ripley: Workflow security + version constant (5 findings)
- Fenster: C++ ownership & const-correctness (16 findings)
- Dallas: GUI move semantics + docs JavaScript (8 findings)
- Hockney (you): Shell script modernization (21+ findings)

**Status:** All agent deliverables integrated; baseline build passed; zero conflicts between scopes.

**Validation Completed:** Shell syntax validated with Git Bash; installation behavior preserved; build script hardening for `set -u` verified.

**Next:** Sonar re-scan to confirm all 50+ findings resolved across workflows, C++, JavaScript, and shell scripts.
