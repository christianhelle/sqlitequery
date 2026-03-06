# Squad Setup Guide - SQLite Query Analyzer

## Overview

Squad is an AI agent coordination system configured for the SQLite Query Analyzer project. This guide explains how Squad is set up to work with this Qt6 C++ codebase and how to use it effectively.

**Project**: SQLite Query Analyzer - A cross-platform Qt6 C++ desktop application for managing SQLite databases with GUI and CLI interfaces.

## Configuration Details

### Project Context

The `squad.config.ts` file in the repository root is configured with the following project metadata:

```
Name:        SQLite Query Analyzer
Language:    C++17
Framework:   Qt6
Platforms:   Windows, macOS, Linux
Build System: CMake 3.16+
```

This context helps agents understand the technology stack and provide accurate recommendations specific to this project.

### Model Configuration

Squad uses a three-tier model strategy:

| Tier | Models | Use Case | Example Tasks |
|------|--------|----------|----------------|
| **Premium** | Claude Opus 4.6 family | Complex decisions, architecture | Design Qt signal/slot patterns, optimize database queries, cross-platform issues |
| **Standard** | Claude Sonnet 4.5 (default) | Development work | Feature implementation, bug fixes, CMake updates |
| **Fast** | Haiku 4.5 (default fallback) | Quick tasks | Documentation, simple fixes, validation |

The `preferSameProvider` setting ensures model consistency, and fallback chains provide reliability if a model is unavailable.

### Agent Routing Rules

Work types are automatically routed to appropriate agents with specific model tiers:

| Work Type | Agents | Tier | Purpose |
|-----------|--------|------|---------|
| `feature-dev` | @scribe | Standard | GUI enhancements, database operations, CLI additions |
| `bug-fix` | @scribe | Standard | Cross-platform bug fixes and issue resolution |
| `testing` | @scribe | Fast | Manual validation, CLI testing, cross-platform verification |
| `documentation` | @scribe | Fast | README updates, build guides, code documentation |
| `build-system` | @scribe | Standard | CMake configuration, CPack packaging, cross-platform build scripts |
| `code-review` | @scribe | Premium | Code quality review, architecture assessment |
| `architecture` | @scribe | Premium | Design decisions, module restructuring |

## Using Squad with SQLite Query Analyzer

### Directory Structure Context

Squad agents are aware of the project structure:

```
src/
├── gui/              - Qt GUI components (mainwindow.cpp, UI layouts)
├── database/         - Core database operations (schema, queries, export)
├── cli/              - Command-line interface (CSV export, script execution)
├── project/          - Build system (CMake, build scripts)
├── settings/         - Application preferences and persistence
└── threading/        - Concurrency utilities (cancellation, thread safety)
```

### Key Build Commands

Squad agents know the standard build workflow:

```bash
# Configure and build
cd src/project
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

# Test CLI functionality (headless)
QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --help
```

### Code Style

Agents are configured to:
- Follow **C++17 standard**
- Use **clang-format** for code formatting (LLVM style, 2-space indentation)
- Format configuration: `src/.clang-format`

Before committing, format with:
```bash
clang-format -i src/**/*.cpp src/**/*.h
```

## Development Workflows

### Feature Development

**When to use**: Adding new functionality (e.g., new export format, GUI enhancement)

```
Squad Instruction: "I'm adding a feature to SQLite Query Analyzer"
Expected: @scribe agent routes to standard-tier model
```

**Typical steps**:
1. Create feature branch from master
2. Implement in appropriate module (gui/, database/, or cli/)
3. Update CMakeLists.txt if needed
4. Test with build script
5. Commit with descriptive message
6. Push for review

### Bug Fixes

**When to use**: Fixing issues in existing code

```
Squad Instruction: "I need to fix a bug in SQLite Query Analyzer"
Expected: @scribe agent routes to standard-tier model
```

**Typical steps**:
1. Create bugfix branch
2. Locate issue in appropriate module
3. Implement fix
4. Test across platforms (at minimum, Linux/Windows/macOS)
5. Verify with CLI tests
6. Commit with issue reference
7. Push for review

### Cross-Platform Testing

**When to use**: Validating changes work on all platforms

```
Squad Instruction: "I need to test this on Windows, macOS, and Linux"
Expected: @scribe agent routes to fast-tier model for validation steps
```

**Key platforms**:
- **Linux**: GCC, Qt6 from apt, CMake build
- **Windows**: MSVC 2022, Qt 6.9.0, Visual Studio
- **macOS**: Clang, Qt6 from Homebrew, XCode

### Code Quality Review

**When to use**: Comprehensive code review before merge

```
Squad Instruction: "Please review this code for quality and performance"
Expected: @scribe agent routes to premium-tier model
```

**Review focus areas**:
- Architecture and design patterns
- Performance optimization opportunities
- Memory safety and resource management
- Cross-platform compatibility
- Qt best practices (signals/slots, memory models)

## Agent Capabilities for This Project

Squad agents are configured with knowledge of:

✓ **C++17** programming and best practices
✓ **Qt6** framework (GUI, SQL, Core, Widgets, Concurrent)
✓ **CMake** build system and cross-platform configuration
✓ **SQLite3** database operations and schema design
✓ **Cross-platform development** (Windows/macOS/Linux)
✓ **GUI design** with Qt Designer (.ui files)
✓ **Command-line interfaces** and argument parsing
✓ **Build automation** with CPack (7Z, ZIP, DEB, RPM, Snap)
✓ **Code formatting** with clang-format (LLVM style)
✓ **Git workflow** with feature branches and atomic commits

## Communication Style

Squad is configured with character and communication preferences from:
- The Usual Suspects
- Breaking Bad
- The Wire
- Firefly

This ensures consistent, engaging communication when working with agents while maintaining professionalism.

## Platform-Specific Considerations

### Linux (Ubuntu 24.04)

- **Compiler**: GCC 13.3+
- **Qt**: `qt6-base-dev` package
- **Build**: CMake + make
- **Packaging**: DEB, RPM, 7Z, Snap
- **Test**: `QT_QPA_PLATFORM=offscreen` for CLI testing

### macOS

- **Compiler**: Clang (XCode)
- **Qt**: Homebrew or official installer
- **Build**: CMake + Xcode or ninja
- **Packaging**: App bundles, DMG images
- **Deploy**: `macdeployqt` for app bundle

### Windows

- **Compiler**: MSVC 2022
- **Qt**: Official installer (6.9.0)
- **Build**: CMake + Visual Studio
- **Packaging**: NSIS/Inno Setup, ZIP, 7Z
- **Deploy**: `windeployqt` for runtime dependencies

## Best Practices for Working with Squad

### 1. Provide Complete Context

When working with agents, include:
- What you're trying to accomplish
- Which module it affects (gui, database, cli, etc.)
- Platform considerations if applicable
- Any constraints or requirements

### 2. Test Before Committing

Always validate changes:
```bash
# Build
cd src/project && cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Test CLI
QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --help

# Test specific functionality
./linux/bin/SQLiteQueryAnalyzer database.sqlite
```

### 3. Use Descriptive Commit Messages

```
Good:   "Add CSV export with custom delimiter support"
Good:   "Fix memory leak in database connection pooling"
Good:   "Update CMake for Qt6.9.0 on Windows MSVC"
Bad:    "Fix bug"
Bad:    "Update code"
```

### 4. Format Before Committing

```bash
find src -name '*.cpp' -o -name '*.h' | xargs clang-format -i
```

### 5. Test Cross-Platform

At minimum, verify changes work on:
- Linux (primary development platform)
- Windows (handles MSVC/Qt SDK differences)
- macOS (handles Clang/Homebrew differences)

## Troubleshooting

### "could not connect to display" error

**Cause**: Trying to run GUI app in headless environment
**Solution**: Use `QT_QPA_PLATFORM=offscreen` for CLI testing

### Qt not found in CMake

**Cause**: Qt6 not installed or CMake can't locate it
**Solution**: Specify `CMAKE_PREFIX_PATH` or install `qt6-base-dev`

### Build takes longer than expected

**Cause**: Build not parallelizing or running on slow hardware
**Solution**: Use `cmake --build . --parallel $(nproc)` or increase parallel jobs

## Additional Resources

- **Repository**: https://github.com/christianhelle/sqlitequery
- **README**: See README.md for build instructions
- **Qt Documentation**: https://doc.qt.io/qt-6/
- **CMake Documentation**: https://cmake.org/documentation/

## Support

For issues or questions about Squad setup for this project:
1. Check this guide (SQUAD_SETUP.md)
2. Review squad.config.ts for configuration details
3. Check recent commits for examples of working with Squad
4. Open an issue on GitHub if you find configuration problems

---

**Last Updated**: March 2026
**Config Version**: 1.0.0
**Project**: SQLite Query Analyzer
