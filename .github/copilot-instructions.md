# SQLite Query Analyzer

SQLite Query Analyzer is a cross-platform Qt6-based C++ desktop application that provides both GUI and CLI interfaces for managing SQLite databases. It supports Linux, Windows, and macOS platforms.

Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.

## Working Effectively

### Bootstrap and build the repository:
- Install dependencies: `sudo apt-get update && sudo apt-get install -y qt6-base-dev libxkbcommon-dev build-essential cmake`
- Navigate to build directory: `cd src/project`
- Configure build: `cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./linux/`
- Build: `cmake --build build --config Release --parallel $(nproc)` -- takes 12 seconds. NEVER CANCEL. Set timeout to 60+ seconds.
- Install: `cmake --install build` -- takes less than 1 second.

### Alternative build using provided scripts:
- **Linux/macOS**: `./build.sh` -- takes 12 seconds. NEVER CANCEL. Set timeout to 60+ seconds.
- **Windows**: `pwsh build.ps1` -- similar timing. NEVER CANCEL. Set timeout to 60+ seconds.

### Run the application:
- **CLI Help**: `QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --help`
- **CLI Version**: `QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --version`
- **Export CSV**: `QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --export-csv database.sqlite`
- **Execute SQL Script**: `QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --run-sql script.sql database.sqlite`
- **GUI Mode**: Requires X11 display - use `./linux/bin/SQLiteQueryAnalyzer database.sqlite` in environments with GUI support

### Package the application:
- **7Z Archive**: `cpack -G 7Z --config ./build/CPackConfig.cmake` -- takes 1 second. NEVER CANCEL. Set timeout to 30+ seconds.
- **ZIP Archive**: `cpack -G ZIP --config ./build/CPackConfig.cmake` -- takes 1 second. NEVER CANCEL. Set timeout to 30+ seconds.
- **Debian Package**: `cpack -G DEB --config ./build/CPackConfig.cmake` -- takes 1 second. NEVER CANCEL. Set timeout to 30+ seconds.
- **RPM Package**: `cpack -G RPM --config ./build/CPackConfig.cmake` -- takes 1 second. NEVER CANCEL. Set timeout to 30+ seconds.

## Validation

### Always manually validate CLI functionality:
- Create test database: `sqlite3 /tmp/testdb.sqlite "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT); INSERT INTO users (name, email) VALUES ('Test User', 'test@example.com');"`
- Test CSV export: `QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --export-csv /tmp/testdb.sqlite`
- Verify export: `ls -la *.csv && head *.csv`
- Test SQL execution: Create SQL file with `echo "SELECT COUNT(*) FROM users;" > /tmp/test.sql` then run `QT_QPA_PLATFORM=offscreen ./linux/bin/SQLiteQueryAnalyzer --run-sql /tmp/test.sql /tmp/testdb.sqlite`

### Code formatting validation:
- Always run `clang-format --dry-run --Werror src/**/*.cpp src/**/*.h` to check formatting issues before committing
- The codebase currently has formatting violations - use `clang-format -i src/**/*.cpp src/**/*.h` to fix them if needed
- Format configuration is in `src/.clang-format` using LLVM style

### NO TESTING FRAMEWORK:
- This project does not have unit tests or a formal testing framework
- Validation is done through manual testing of CLI and GUI functionality
- Always test actual functionality after making changes

## Common Tasks

### Repository structure overview:
```
src/
├── main.cpp              # Application entry point with CLI argument parsing
├── project/              # Build configuration and scripts
│   ├── CMakeLists.txt   # Main CMake configuration
│   ├── build.sh         # Linux/macOS build script
│   ├── build.ps1        # Windows/cross-platform PowerShell build script
│   └── snapcraft.yaml  # Snap package configuration
├── gui/                  # Qt GUI components
│   ├── mainwindow.cpp   # Main application window
│   ├── mainwindow.ui    # UI layout file
│   └── highlighter.cpp # SQL syntax highlighting
├── database/            # Database operations
│   ├── database.cpp     # Core database functionality
│   ├── dbexport.cpp     # CSV export functionality
│   └── dbquery.cpp      # SQL query execution
├── cli/                 # Command-line interface
│   ├── export.cpp       # CLI export functionality
│   └── script.cpp       # CLI script execution
└── settings/            # Application settings and preferences
```

### Key build systems:
- **Primary**: CMake with Qt6 integration
- **Legacy**: QMake (.pro file) for older Qt compatibility
- **Packaging**: CPack for multiple formats (7Z, ZIP, DEB, RPM, Snap)

### Platform-specific notes:
- **Linux**: Uses qt6-base-dev, builds with GCC
- **Windows**: Requires Visual Studio, Qt 6.9.0 MSVC2022, uses windeployqt
- **macOS**: Uses Qt6 from Homebrew or official installer, uses macdeployqt for app bundles

### CLI Usage Examples:
```bash
# Show help
QT_QPA_PLATFORM=offscreen ./SQLiteQueryAnalyzer --help

# Export all tables to CSV in current directory
QT_QPA_PLATFORM=offscreen ./SQLiteQueryAnalyzer --export-csv database.db

# Export with progress indicator  
QT_QPA_PLATFORM=offscreen ./SQLiteQueryAnalyzer --export-csv --progress database.db

# Execute SQL script
QT_QPA_PLATFORM=offscreen ./SQLiteQueryAnalyzer --run-sql queries.sql database.db

# Open GUI (requires display)
./SQLiteQueryAnalyzer database.db
```

### Build timing expectations:
- **Configure**: 2-3 seconds
- **Build**: 12 seconds on modern hardware
- **Install**: <1 second
- **Package creation**: 1 second per format
- **Total from clean**: ~15 seconds

### CRITICAL: Always use QT_QPA_PLATFORM=offscreen for CLI testing:
- The application is a Qt GUI app that requires a display
- In headless environments, use `QT_QPA_PLATFORM=offscreen` to run CLI commands
- Without this, the application will fail with "could not connect to display" errors

### Environment requirements:
- **Ubuntu 24.04 LTS**: Fully supported and tested
- **Qt Version**: 6.4.2 minimum (Ubuntu package), 6.9.0 recommended
- **CMake**: 3.16 or later
- **Compiler**: GCC 13.3+ or equivalent
- **Memory**: Build requires ~2GB RAM
- **Disk**: ~100MB for build artifacts

### Troubleshooting common issues:
- **"could not connect to display"**: Add `QT_QPA_PLATFORM=offscreen` for CLI usage
- **Qt not found**: Install `qt6-base-dev` package
- **Build fails**: Ensure `build-essential` and `cmake` are installed
- **Permission denied on install**: Use proper install prefix like `./linux/` instead of system paths