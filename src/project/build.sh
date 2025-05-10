#!/usr/bin/env bash
# build.sh - Build script for SQLiteQueryAnalyzer

# Exit on error
set -e

# Parse arguments
PACKAGE=false
INSTALL=false

while (( "$#" )); do
  case "$1" in
    --package)
      PACKAGE=true
      shift
      ;;
    --install)
      INSTALL=true
      shift
      ;;
    *)
      echo "Unknown argument: $1"
      echo "Usage: $0 [--package] [--install]"
      exit 1
      ;;
  esac
done

# Detect OS
OS=$(uname)

if [ "$OS" = "Linux" ]; then
    echo "Building for Linux..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./linux/
    cmake --build build --config Release --parallel $(nproc)
    cmake --install build

    if [ "$INSTALL" = true ]; then
        mkdir -p ~/.local/bin
        cp -rf ./linux /tmp/sqlitequery
        ln -sf /tmp/sqlitequery/bin/SQLiteQueryAnalyzer ~/.local/bin/sqlitequery
        echo "Installed to ~/.local/bin/sqlitequery"
    fi

    if [ "$PACKAGE" = true ]; then
        echo "Creating packages..."
        cpack -G 7Z --config ./build/CPackConfig.cmake
        cpack -G ZIP --config ./build/CPackConfig.cmake
        cpack -G TBZ2 --config ./build/CPackConfig.cmake
        cpack -G TGZ --config ./build/CPackConfig.cmake
        cpack -G TXZ --config ./build/CPackConfig.cmake
        cpack -G TZ --config ./build/CPackConfig.cmake
        cpack -G DEB --config ./build/CPackConfig.cmake
        cpack -G RPM --config ./build/CPackConfig.cmake
        snapcraft
        echo "Package creation complete"
    fi
fi

if [ "$OS" = "Darwin" ]; then
    echo "Building for macOS..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/sqlitequery
    cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)
    
    if [ "$PACKAGE" = true ]; then
        echo "Creating macOS package..."
        macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
        echo "Package creation complete"
    fi
fi

echo "Build completed successfully"
