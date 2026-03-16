#!/bin/bash

# SQLiteQueryAnalyzer - Installation Script
# This script downloads and installs the latest release of SQLiteQueryAnalyzer

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
GITHUB_REPO="christianhelle/sqlitequery"
INSTALL_DIR="${INSTALL_DIR:-/usr/local/bin}"
BINARY_NAME="SQLiteQueryAnalyzer"

# Functions
log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}" >&2
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}" >&2
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}" >&2
}

log_error() {
    echo -e "${RED}❌ $1${NC}" >&2
}

detect_platform() {
    local os=$(uname -s | tr '[:upper:]' '[:lower:]')
    local arch=$(uname -m)
    
    case "$os" in
        linux*)
            os="linux"
            ;;
        darwin*)
            os="macos"
            ;;
        *)
            log_error "Unsupported operating system: $os"
            exit 1
            ;;
    esac
    
    case "$arch" in
        x86_64|amd64)
            arch="x86_64"
            ;;
        aarch64|arm64)
            arch="arm64"
            ;;
    esac
    
    echo "${os}-${arch}"
}

check_dependencies() {
    local deps=("curl")
    
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" >/dev/null 2>&1; then
            log_error "Required dependency '$dep' not found. Please install it first."
            exit 1
        fi
    done
}

get_latest_release() {
    log_info "Fetching latest release information..."
    local api_url="https://api.github.com/repos/$GITHUB_REPO/releases/latest"
    
    if ! curl -s "$api_url" | grep -o '"tag_name": "[^"]*' | grep -o '[^"]*$'; then
        log_error "Failed to fetch release information"
        exit 1
    fi
}

get_asset_url() {
    local os="$1"
    local version="$2"
    local arch="${3:-}"
    local api_url="https://api.github.com/repos/$GITHUB_REPO/releases/tags/$version"
    local identifier=""
    
    case "$os" in
        linux*)
            identifier="Linux"
            ;;
        macos*)
            if [ -n "$arch" ]; then
                identifier="MacOS ($arch)"
            else
                identifier="MacOS"
            fi
            ;;
    esac
    
    local assets_json
    assets_json=$(curl -s "$api_url")
    
    # Prefer assets whose name contains the identifier (case-insensitive)
    url=$(echo "$assets_json" | awk -v id="$identifier" 'BEGIN{IGNORECASE=1} /"name":/ {name=$0; getline; if (name ~ id && $0 ~ /browser_download_url/) { if (match($0, /"browser_download_url": "([^\"]+)"/, m)) print m[1]; exit }}')
    if [ -n "$url" ]; then
        echo "$url"
        return 0
    fi

    # Fallback: coarse matching by OS keyword
    case "$os" in
        linux*) echo "$assets_json" | grep -o '"browser_download_url": "https://[^\"]*linux[^\"]*"' | grep -o 'https://[^\"]*' | head -1 ;;
        macos*) echo "$assets_json" | grep -o '"browser_download_url": "https://[^\"]*macos[^\"]*"' | grep -o 'https://[^\"]*' | head -1 ;;
        *) echo "" ;;
    esac
}

download_and_install_linux() {
    local platform="$1"
    local version="$2"
    local download_url
    download_url=$(get_asset_url "linux" "$version")
    
    if [ -z "$download_url" ]; then
        log_error "Failed to find Linux release asset"
        exit 1
    fi
    
    local temp_dir=$(mktemp -d)
    local archive_name=$(basename "$download_url")
    
    log_info "Downloading SQLiteQueryAnalyzer $version for $platform..."
    
    if ! curl -L -o "$temp_dir/$archive_name" "$download_url"; then
        log_error "Failed to download SQLiteQueryAnalyzer"
        rm -rf "$temp_dir"
        exit 1
    fi
    
    log_info "Extracting archive..."
    if ! tar -xzf "$temp_dir/$archive_name" -C "$temp_dir"; then
        log_error "Failed to extract archive"
        rm -rf "$temp_dir"
        exit 1
    fi
    
    log_info "Installing to $INSTALL_DIR..."
    
    # Find the binary in extracted files
    local binary_path=""
    for f in "$temp_dir"/*; do
        if [ -f "$f" ] && [ -x "$f" ]; then
            binary_path="$f"
            break
        fi
    done
    
    if [ -z "$binary_path" ]; then
        # Try with common binary names
        if [ -f "$temp_dir/$BINARY_NAME" ]; then
            binary_path="$temp_dir/$BINARY_NAME"
        else
            log_error "Binary not found in archive"
            rm -rf "$temp_dir"
            exit 1
        fi
    fi
    
    # Check if we need sudo
    if [ ! -w "$INSTALL_DIR" ]; then
        if command -v sudo >/dev/null 2>&1; then
            log_warning "Installing with sudo (directory not writable by current user)"
            sudo cp "$binary_path" "$INSTALL_DIR/"
            sudo chmod +x "$INSTALL_DIR/$BINARY_NAME"
        else
            log_error "Cannot write to $INSTALL_DIR and sudo is not available"
            log_info "Try setting INSTALL_DIR to a writable directory:"
            log_info "  curl -fsSL https://christianhelle.com/sqlitequery/install.sh | INSTALL_DIR=\$HOME/.local/bin bash -s --"
            rm -rf "$temp_dir"
            exit 1
        fi
    else
        cp "$binary_path" "$INSTALL_DIR/"
        chmod +x "$INSTALL_DIR/$BINARY_NAME"
    fi
    
    # Cleanup
    rm -rf "$temp_dir"
    
    log_success "SQLiteQueryAnalyzer $version installed successfully!"
}

download_and_install_macos() {
    local arch="$1"
    local version="$2"
    
    local arch_label=""
    
    if [ "$arch" = "arm64" ]; then
        arch_label="ARM64"
    else
        arch_label="Intel"
    fi
    
    local download_url
    download_url=$(get_asset_url "macos" "$version" "$arch_label")
    
    if [ -z "$download_url" ]; then
        log_error "Failed to find macOS release asset"
        exit 1
    fi
    
    local temp_dir=$(mktemp -d)
    local dmg_path="$temp_dir/sqlitequery.dmg"
    local mount_point="/Volumes/SQLiteQueryAnalyzer"
    
    log_info "Downloading SQLiteQueryAnalyzer $version for macOS ($arch_label)..."
    
    if ! curl -L -o "$dmg_path" "$download_url"; then
        log_error "Failed to download SQLiteQueryAnalyzer"
        rm -rf "$temp_dir"
        exit 1
    fi
    
    log_info "Mounting DMG..."
    if ! hdiutil attach "$dmg_path" -nobrowse; then
        log_error "Failed to mount DMG"
        rm -rf "$temp_dir"
        exit 1
    fi
    
    log_info "Installing to /Applications..."
    
    # Find the .app bundle in the mounted volume
    local app_source="$mount_point/SQLiteQueryAnalyzer.app"
    
    if [ ! -d "$app_source" ]; then
        log_error "Application bundle not found in DMG"
        hdiutil detach "$mount_point" 2>/dev/null || true
        rm -rf "$temp_dir"
        exit 1
    fi
    
    # Copy to Applications using atomic staging to avoid leaving a broken install if copy fails
    local staging_dir
    staging_dir=$(mktemp -d)
    log_info "Staging application to temporary directory: $staging_dir"
    if ! cp -R "$app_source" "$staging_dir/"; then
        log_error "Failed to copy application to staging directory"
        hdiutil detach "$mount_point" 2>/dev/null || true
        rm -rf "$temp_dir" "$staging_dir"
        exit 1
    fi

    local staged_app="$staging_dir/SQLiteQueryAnalyzer.app"
    if [ ! -d "$staged_app" ]; then
        log_error "Staged application bundle not found: $staged_app"
        hdiutil detach "$mount_point" 2>/dev/null || true
        rm -rf "$temp_dir" "$staging_dir"
        exit 1
    fi

    # Atomically install: remove old and move staged into /Applications
    if [ -d "/Applications/SQLiteQueryAnalyzer.app" ]; then
        log_info "Removing existing installation..."
        rm -rf "/Applications/SQLiteQueryAnalyzer.app"
    fi

    if ! mv "$staged_app" "/Applications/"; then
        log_error "Failed to move staged application into /Applications"
        rm -rf "$temp_dir" "$staging_dir"
        exit 1
    fi

    rm -rf "$staging_dir"

    log_info "Unmounting DMG..."
    hdiutil detach "$mount_point" 2>/dev/null || true
    
    # Cleanup
    rm -rf "$temp_dir"
    
    log_success "SQLiteQueryAnalyzer $version installed successfully!"
}

verify_installation() {
    if command -v "$BINARY_NAME" >/dev/null 2>&1; then
        log_success "Installation verified"
        log_info "You can now launch: $BINARY_NAME"
    else
        log_warning "Binary installed but not found in PATH"
        log_info "Make sure $INSTALL_DIR is in your PATH"
        log_info "Add this to your shell profile: export PATH=\"$INSTALL_DIR:\$PATH\""
    fi
}

verify_macos_installation() {
    if [ -d "/Applications/SQLiteQueryAnalyzer.app" ]; then
        log_success "Installation verified"
        log_info "You can find SQLiteQueryAnalyzer in /Applications or Launchpad"
    else
        log_warning "Application not found in /Applications"
    fi
}

show_usage() {
    echo "SQLiteQueryAnalyzer Installation Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help      Show this help message"
    echo "  -d, --dir DIR   Set installation directory (default: /usr/local/bin)"
    echo ""
    echo "Environment variables:"
    echo "  INSTALL_DIR     Installation directory (default: /usr/local/bin)"
    echo ""
    echo "Examples:"
    echo "  # Install to default location"
    echo "  curl -fsSL https://christianhelle.com/sqlitequery/install.sh | bash"
    echo ""
    echo "  # Install to custom directory (set INSTALL_DIR in the receiving shell)"
    echo "  curl -fsSL https://christianhelle.com/sqlitequery/install.sh | INSTALL_DIR=\$HOME/.local/bin bash -s --"
    echo ""
    echo "  # Install to custom directory using flag"
    echo "  curl -fsSL https://christianhelle.com/sqlitequery/install.sh | bash -s -- --dir \$HOME/.local/bin"
}

main() {
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -d|--dir)
                if [ "$#" -lt 2 ]; then
                    log_error "Missing argument for -d/--dir option"
                    show_usage
                    exit 1
                fi
                if [[ "$2" == -* ]]; then
                    log_error "Missing argument for -d/--dir option"
                    show_usage
                    exit 1
                fi
                INSTALL_DIR="$2"
                shift 2
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    log_info "Starting SQLiteQueryAnalyzer installation..."
    
    # Detect platform
    local platform=$(detect_platform)
    log_info "Detected platform: $platform"
    
    # Check dependencies
    check_dependencies
    
    # Get latest release
    local version=$(get_latest_release)
    log_info "Latest version: $version"
    
    # Create install directory if it doesn't exist (for Linux)
    if [[ "$platform" == linux-* ]]; then
        log_info "Target directory: $INSTALL_DIR"
        if [ ! -d "$INSTALL_DIR" ]; then
            log_info "Creating installation directory: $INSTALL_DIR"
            if ! mkdir -p "$INSTALL_DIR" 2>/dev/null; then
                if command -v sudo >/dev/null 2>&1; then
                    sudo mkdir -p "$INSTALL_DIR"
                else
                    log_error "Cannot create directory $INSTALL_DIR"
                    exit 1
                fi
            fi
        fi
        
        # Download and install
        download_and_install_linux "$platform" "$version"
        
        # Verify installation
        verify_installation
    else
        # macOS
        local arch=$(echo "$platform" | cut -d'-' -f2)
        
        # Download and install
        download_and_install_macos "$arch" "$version"
        
        # Verify installation
        verify_macos_installation
    fi
    
    echo ""
    log_success "🎉 Installation complete!"
    log_info "Get started with: SQLiteQueryAnalyzer"
    log_info "Documentation: https://christianhelle.com/sqlitequery/"
}

# Run main function with all arguments
main "$@"
