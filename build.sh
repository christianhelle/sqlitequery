#!/usr/bin/env bash
# build.sh - Build script for SQLiteQueryAnalyzer

# Exit on error
set -e

# Parse arguments
PACKAGE=false
INSTALL=false

while [[ "$#" -gt 0 ]]; do
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

# Check for Windows with MSYS2/Git Bash
if [[ "$OS" == MINGW* ]] || [[ "$OS" == CYGWIN* ]]; then
  echo "This script is not designed for Windows. Please use build.ps1 instead."
  exit 1
fi

if [[ "$OS" != "Linux" ]] && [[ "$OS" != "Darwin" ]]; then
  echo "Unsupported operating system: $OS"
  echo "This script supports Linux and macOS only."
  exit 1
fi

if [[ "$OS" == "Linux" ]]; then
  echo "Building for Linux..."
  cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./linux/
  cmake --build build --config Release --parallel "$(nproc)"
  cmake --install build

  if [[ "$INSTALL" == true ]]; then
    mkdir -p ~/.local/bin
    rm -rf /tmp/sqlitequery
    mkdir -p /tmp/sqlitequery
    cp -rf ./linux/* /tmp/sqlitequery
    ln -sf /tmp/sqlitequery/bin/SQLiteQueryAnalyzer ~/.local/bin/sqlitequery
    echo "Installed to ~/.local/bin/sqlitequery"
  fi

  if [[ "$PACKAGE" == true ]]; then
    echo "Creating packages..."
    for GEN in 7Z ZIP TBZ2 TGZ TXZ TZ DEB RPM; do
      cpack -G "$GEN" --config ./build/CPackConfig.cmake
    done

    if [[ "${DISABLE_SNAP:-}" == "true" ]]; then
      echo "Snap package creation skipped (DISABLE_SNAP=true)."
    elif command -v snapcraft &>/dev/null; then
      echo "Creating snap package..."
      snapcraft
    else
      echo "snapcraft not found. Snap package will not be created."
      echo "To install snapcraft, run: sudo snap install snapcraft --classic"
      echo "Alternatively, build via Docker:"
      echo "  docker run --rm -v \$(pwd):/build -w /build snapcore/snapcraft:stable snapcraft"
    fi

    echo "Package creation complete"
  fi
fi

if [[ "$OS" == "Darwin" ]]; then
  echo "Building for macOS..."
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release --parallel "$(sysctl -n hw.ncpu)"

  if [[ "$INSTALL" == true ]]; then
    mkdir -p ~/.local/bin
    ln -sf "$(pwd)/build/SQLiteQueryAnalyzer.app/Contents/MacOS/SQLiteQueryAnalyzer" ~/.local/bin/sqlitequery
    echo "Installed to ~/.local/bin/sqlitequery"
  fi

  if [[ "$PACKAGE" == true ]]; then
    echo "Creating macOS package..."
    macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    echo "Package creation complete"
  fi
fi

echo "Build completed successfully"
