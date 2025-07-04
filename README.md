[![Linux](https://github.com/christianhelle/sqlitequery/actions/workflows/linux.yml/badge.svg)](https://github.com/christianhelle/sqlitequery/actions/workflows/linux.yml)
[![MacOS](https://github.com/christianhelle/sqlitequery/actions/workflows/macos.yml/badge.svg)](https://github.com/christianhelle/sqlitequery/actions/workflows/macos.yml)
[![Windows](https://github.com/christianhelle/sqlitequery/actions/workflows/windows.yml/badge.svg)](https://github.com/christianhelle/sqlitequery/actions/workflows/windows.yml)

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=christianhelle_sqlitequery&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=christianhelle_sqlitequery)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=christianhelle_sqlitequery&metric=bugs)](https://sonarcloud.io/summary/new_code?id=christianhelle_sqlitequery)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=christianhelle_sqlitequery&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=christianhelle_sqlitequery)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=christianhelle_sqlitequery&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=christianhelle_sqlitequery)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=christianhelle_sqlitequery&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=christianhelle_sqlitequery)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=christianhelle_sqlitequery&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=christianhelle_sqlitequery)

# SQLite Query Analyzer

SQLite Query Analyzer is a lightweight and efficient desktop utility designed
to simplify the process of managing SQLite databases.

It provides an intuitive interface for executing queries and direct table editing,
making database operations seamless and straightforward.

## Features

- Cross platform - Runs natively on Windows, MacOS, and Linux
- Easy-to-use interface for executing SQL queries
- Fast table data editing
- Persists application state and reloads last session on startup
- Export database schema as CREATE TABLE statements
- Export data as an SQL script containing INSERT statements or as CSV files
- Desktop color theme awareness with automatic switching between dark/light themes
- Command line interface for automation and scripting

## CLI Usage

SQLite Query Analyzer can be used as a command line tool for automating database operations without the GUI. The application supports several command line options for exporting data, executing SQL scripts, and more.

### Help Text

```sh
$ sqlitequery --help
Usage: /snap/sqlitequery/3/bin/SQLiteQueryAnalyzer [options] database
A fast and lightweight cross-platform command line and GUI tool for querying and manipulating SQLite databases

Options:
  -h, --help              Displays help on commandline options.
  --help-all              Displays help, including generic Qt options.
  -v, --version           Displays version information.
  -p, --progress          Show progress during copy
  -e, --export-csv        Export data to CSV.
  -d, --target-directory  Target directory for export.
  -r, --run-sql           Execute SQL file.

Arguments:
  database                Database file to open.
```

### Usage Examples

#### Opening a database in GUI mode
```sh
sqlitequery /path/to/database.db
```

#### Exporting data to CSV files
```sh
# Export all tables to CSV files in the current directory
sqlitequery --export-csv /path/to/database.db

# Export with progress indicator
sqlitequery --export-csv --progress /path/to/database.db

# Export to a specific directory
sqlitequery --export-csv --target-directory /path/to/export/folder /path/to/database.db
```

#### Executing SQL scripts
```sh
# Execute a SQL script file against a database
sqlitequery --run-sql /path/to/script.sql /path/to/database.db
```

### CLI Features

- **Export to CSV**: Export all database tables to individual CSV files
- **Execute SQL Scripts**: Run SQL scripts from files against a database
- **Progress Reporting**: Show progress indicators for long-running operations
- **Flexible Output**: Specify custom directories for exported files

## Screenshots

Here are some screenshots of SQLite Query Analyzer in action:

## Windows

![Insert query](images/windows-query-insert.png)
![Select query](images/windows-query-select.png)
![Table data editing](images/windows-table-data.png)
![Dark Mode Insert query](images/windows-dark-query-insert.png)
![Dark Mode Select query](images/windows-dark-query-select.png)
![Dark Mode Table data editing](images/windows-dark-table-data.png)

## MacOS

![Insert query](images/mac-query-insert.png)
![Select query](images/mac-query-select.png)
![Table data editing](images/mac-table-data.png)
![Dark Mode Insert query](images/mac-dark-query-insert.png)
![Dark Mode Select query](images/mac-dark-query-select.png)
![Dark Mode Table data editing](images/mac-dark-table-data.png)

## Linux (Ubuntu)

![Insert query](images/linux-query-insert.png)
![Select query](images/linux-query-select.png)
![Table data editing](images/linux-table-data.png)
![Dark Mode Insert query](images/linux-dark-query-insert.png)
![Dark Mode Select query](images/linux-dark-query-select.png)
![Dark Mode Table data editing](images/linux-dark-table-data.png)

## Building

### Prerequisites

- Git 😄
- CMake 3.16 or later - Install from [official website](https://cmake.org/download/)
- Qt 6.9.0 - Install from [official website](https://www.qt.io/download-qt-installer-oss)
- [Powershell Core](https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell) (Optional)

### Clone the repository

```sh
git clone https://github.com/christianhelle/sqlitequery.git
cd sqlitequery
```

### Build the project using cross platform powershell script

```sh
cd src/project
pwsh build.ps1
```

The build output folder is under `src/project/build` for MacOS and Linux, and `src/project/build/Release` for Windows

### Building on Linux

Install CMAke and Qt6 and XKB

```sh
sudo apt-get update
sudo apt-get install -y cmake qt6-base-dev libxkbcommon-dev
```

Build project

```sh
cd src/project
cmake .
cmake --build . --config Release
```

### Building on MacOS

Install CMake and Qt6. It's recommended to install Qt using the [official installer](https://www.qt.io/download-qt-installer-oss)
```sh
brew update
brew install cmake
brew install qt@6
```

Build project

```sh
cd src/project
cmake .
cmake --build . --config Release
```

Build MacOS disk image (Optional). 

There is a bug in the Homebrew distribution of Qt which causes the use of `macdeployqt` to fail.

```sh
macdeployqt SQLiteQueryAnalyzer.app -dmg
```

### Building on Windows

Build the project (These instructions assumes that Qt root folder is C:\Qt)

```pwsh
cd src
cmake . -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/msvc2022_64 -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /permissive-" -B build
cmake --build build --config Release
C:\Qt\6.9.0\msvc2022_64\bin\windeployqt.exe .\build\Release\SQLiteQueryAnalyzer.exe
```

Build the installer project using Inno Setup (Optional)

```pwsh
../deps/innosetup/ISCC.exe setup.iss
```

## Contributing

We welcome contributions to SQLite Query Analyzer!
If you have any ideas, suggestions, or bug reports,
please open an issue or submit a pull request on GitHub.
