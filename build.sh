#!/usr/bin/env bash
# build.sh - Build script for SQLiteQueryAnalyzer

# Exit on error
set -e

# Parse arguments
PACKAGE=false
INSTALL=false
QT_PATH="${QT_PATH:-}"

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
  --qt-path)
    QT_PATH="$2"
    shift 2
    ;;
  --qt-path=*)
    QT_PATH="${1#*=}"
    shift
    ;;
  *)
    echo "Unknown argument: $1"
    echo "Usage: $0 [--package] [--install] [--qt-path PATH]"
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

# Auto-detect Qt installation if not supplied
detect_qt_path() {
  local subdir
  if [[ "$OS" == "Linux" ]]; then
    subdir="gcc_64"
  else
    subdir="macos"
  fi

  local base
  for base in "$HOME/Qt" "/opt/Qt"; do
    if [[ -d "$base" ]]; then
      local version
      version=$(ls -1 "$base" 2>/dev/null | grep -E '^[0-9]+\.[0-9]+' | sort -V | tail -1)
      if [[ -n "$version" ]] && [[ -d "$base/$version/$subdir" ]]; then
        echo "$base/$version/$subdir"
        return 0
      fi
    fi
  done

  if [[ "$OS" == "Darwin" ]]; then
    for base in /opt/homebrew/opt/qt /opt/homebrew/opt/qt@6 /usr/local/opt/qt /usr/local/opt/qt@6; do
      if [[ -d "$base" ]]; then
        echo "$base"
        return 0
      fi
    done
  fi

  return 1
}

if [[ -z "$QT_PATH" ]]; then
  QT_PATH=$(detect_qt_path || true)
fi

if [[ -z "$QT_PATH" ]]; then
  echo "Qt installation not found in \$HOME/Qt or /opt/Qt."
  echo "Relying on system Qt; if cmake fails, install Qt and retry. Examples:"
  if [[ "$OS" == "Linux" ]]; then
    echo "  sudo apt-get install -y qt6-base-dev libxkbcommon-dev"
  else
    echo "  brew install qt"
  fi
  echo "Or pass an explicit location: $0 --qt-path /path/to/qt (or QT_PATH=...)."
fi

if [[ "$OS" == "Linux" ]]; then
  echo "Building for Linux..."
  CMAKE_ARGS=(-B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./linux/)
  if [[ -n "$QT_PATH" ]]; then
    echo "Using Qt at: $QT_PATH"
    CMAKE_ARGS+=(-DCMAKE_PREFIX_PATH="$QT_PATH")
  fi
  cmake "${CMAKE_ARGS[@]}"
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
      snapcraft pack
    else
      echo "snapcraft not found. Snap package will not be created."
      echo "To install snapcraft, run: sudo snap install snapcraft --classic"
      echo "snapcraft builds the core24 snap using LXD; install and initialise it with:"
      echo "  sudo snap install lxd && sudo lxd init --auto"
    fi

    echo "Package creation complete"
  fi
fi

if [[ "$OS" == "Darwin" ]]; then
  echo "Building for macOS..."
  CMAKE_ARGS=(-B build -DCMAKE_BUILD_TYPE=Release)
  if [[ -n "$QT_PATH" ]]; then
    echo "Using Qt at: $QT_PATH"
    CMAKE_ARGS+=(-DCMAKE_PREFIX_PATH="$QT_PATH")
  fi
  cmake "${CMAKE_ARGS[@]}"
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
