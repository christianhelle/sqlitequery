#!/usr/bin/env bash
# build.sh - Build script for SQLiteQueryAnalyzer

# Exit on error
set -e

# Parse arguments
PACKAGE=false
INSTALL=false

while [ "$#" -gt 0 ]; do
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
    cp -rf ./linux/* /tmp/sqlitequery
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

    # Check if snap package creation is disabled via the DISABLE_SNAP environment variable
    if [ "${DISABLE_SNAP}" = "true" ]; then
      echo "Snap package creation is disabled (DISABLE_SNAP=true)."
      echo "To create snap packages on a host system, use one of these approaches:"
      echo ""
      echo "Option 1: Use the snapcraft Docker image (recommended):"
      echo "  docker run --rm -v \$(pwd):/build -w /build snapcore/snapcraft:stable snapcraft"
      echo ""
      echo "Option 2: On an Ubuntu host with snapd properly running:"
      echo "  sudo snap install snapcraft --classic"
      echo "  snapcraft"
      echo ""
    else
      # For standard non-container environments
      echo "Checking for snapcraft dependencies..."

      # Check for snapd first
      if ! command -v snap &>/dev/null; then
        echo "Installing snap..."
        sudo apt update
        sudo apt install -y snapd
        # Ensure snapd socket is available
        if ! systemctl is-active snapd.socket &>/dev/null; then
          echo "Starting snapd.socket..."
          sudo systemctl start snapd.socket
          sleep 2
        fi
      fi

      echo "Snap package creation is disabled in this version of the build script."
      echo "To create snap packages on a host system, use one of these approaches:"
      echo ""
      echo "Option 1: Use the snapcraft Docker image (recommended):"
      echo "  docker run --rm -v \$(pwd):/build -w /build snapcore/snapcraft:stable snapcraft"
      echo ""
      echo "Option 2: On an Ubuntu host with snapd properly running:"
      echo "  sudo snap install snapcraft --classic"
      echo "  snapcraft"
      echo ""
    fi

    echo "Package creation complete"
  fi
fi

if [ "$OS" = "Darwin" ]; then
  echo "Building for macOS..."
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)

  if [ "$PACKAGE" = true ]; then
    echo "Creating macOS package..."
    macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    echo "Package creation complete"
  fi
fi

echo "Build completed successfully"
