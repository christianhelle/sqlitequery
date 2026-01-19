#!/usr/bin/env bash
# build.sh - Build script for SQLite Query Analyzer
#
# Usage:
#   ./build.sh [OPTIONS]
#
# Options:
#   --package              Create installer/package after build
#   --install              Install to ~/.local/bin (Linux only)
#   --configuration <cfg>  Build configuration: Release (default) or Debug
#   --debug                Shortcut for --configuration Debug
#
# Examples:
#   ./build.sh                           # Build in Release mode (default)
#   ./build.sh --debug                   # Build in Debug mode
#   ./build.sh --configuration Debug     # Build in Debug mode (alternative)
#   ./build.sh --package                 # Build Release and create packages
#   ./build.sh --install                 # Build Release and install locally
#   ./build.sh --debug --install         # Build Debug and install locally

# Exit on error
set -e

# Parse arguments
PACKAGE=false
INSTALL=false
CONFIGURATION="Release"

show_help() {
  cat <<'EOF'
build.sh - Build script for SQLite Query Analyzer

Usage:
  ./build.sh [OPTIONS]

Options:
  --package              Create installer/package after build
  --install              Install to ~/.local/bin (Linux only)
  --configuration <cfg>  Build configuration: Release (default) or Debug
  --debug                Shortcut for --configuration Debug
  --help                 Show this help message

Examples:
  ./build.sh                           # Build in Release mode (default)
  ./build.sh --debug                   # Build in Debug mode
  ./build.sh --configuration Debug     # Build in Debug mode (alternative)
  ./build.sh --package                 # Build Release and create packages
  ./build.sh --install                 # Build Release and install locally
  ./build.sh --debug --install         # Build Debug and install locally
EOF
  exit 0
}

while [ "$#" -gt 0 ]; do
  case "$1" in
  --help | -h)
    show_help
    ;;
  --package)
    PACKAGE=true
    shift
    ;;
  --install)
    INSTALL=true
    shift
    ;;
  --configuration)
    CONFIGURATION="$2"
    shift 2
    ;;
  --debug)
    CONFIGURATION="Debug"
    shift
    ;;
  *)
    echo "Unknown argument: $1"
    echo "Usage: $0 [--package] [--install] [--configuration <Release|Debug>] [--debug] [--help]"
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

if [ "$OS" = "Linux" ]; then
  echo "Building for Linux..."
  echo "Build Configuration: $CONFIGURATION"
  cmake -B build -DCMAKE_BUILD_TYPE=$CONFIGURATION -DCMAKE_INSTALL_PREFIX=./linux/
  cmake --build build --config $CONFIGURATION --parallel $(nproc)
  cmake --install build

  if [ "$INSTALL" = true ]; then
    mkdir -p ~/.local/bin
    rm -rf /tmp/sqlitequery
    mkdir -p /tmp/sqlitequery
    cp -rf ./linux/* /tmp/sqlitequery
    rm -f ~/.local/bin/sqlitequery
    ln -sf /tmp/sqlitequery/bin/SQLiteQueryAnalyzer ~/.local/bin/sqlitequery
    echo "Installed to ~/.local/bin/sqlitequery"
  fi

  if [ "$PACKAGE" = true ]; then
    echo "Creating packages..."
    for GEN in 7Z ZIP TBZ2 TGZ TXZ TZ DEB RPM; do
      cpack -G "$GEN" --config ./build/CPackConfig.cmake
    done

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
  echo "Build Configuration: $CONFIGURATION"
  cmake -B build -DCMAKE_BUILD_TYPE=$CONFIGURATION
  cmake --build build --config $CONFIGURATION --parallel "$(sysctl -n hw.ncpu)"

  if [ "$PACKAGE" = true ]; then
    echo "Creating macOS package..."
    macdeployqt build/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
    echo "Package creation complete"
  fi
fi

echo "Build completed successfully"
