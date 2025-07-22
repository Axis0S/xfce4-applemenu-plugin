#!/bin/bash
#
# XFCE Apple Menu Plugin Installation Script
# Copyright (C) 2024 Kamil 'Novik' Nowicki <novik@axisos.org>
#
# This script auto-detects the Linux distribution and installs the plugin
# into the appropriate system directories.

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        print_error "This script must be run as root (use sudo)"
        exit 1
    fi
}

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        DISTRO_LIKE=$ID_LIKE
        DISTRO_VERSION=$VERSION_ID
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
        DISTRO_VERSION=$(cat /etc/debian_version)
    elif [ -f /etc/redhat-release ]; then
        DISTRO="rhel"
        DISTRO_VERSION=$(cat /etc/redhat-release | grep -oE '[0-9]+\.[0-9]+')
    else
        print_error "Cannot detect Linux distribution"
        exit 1
    fi
    
    print_info "Detected distribution: $DISTRO (version: $DISTRO_VERSION)"
    
    # Normalize distribution names
    case "$DISTRO" in
        ubuntu|debian|linuxmint|pop|elementary)
            DISTRO_FAMILY="debian"
            ;;
        fedora|rhel|centos|almalinux|rocky)
            DISTRO_FAMILY="redhat"
            ;;
        arch|manjaro|endeavouros)
            DISTRO_FAMILY="arch"
            ;;
        opensuse*|suse*)
            DISTRO_FAMILY="suse"
            ;;
        *)
            # Check ID_LIKE for derivatives
            if [[ "$DISTRO_LIKE" == *"debian"* ]]; then
                DISTRO_FAMILY="debian"
            elif [[ "$DISTRO_LIKE" == *"rhel"* ]] || [[ "$DISTRO_LIKE" == *"fedora"* ]]; then
                DISTRO_FAMILY="redhat"
            elif [[ "$DISTRO_LIKE" == *"arch"* ]]; then
                DISTRO_FAMILY="arch"
            elif [[ "$DISTRO_LIKE" == *"suse"* ]]; then
                DISTRO_FAMILY="suse"
            else
                DISTRO_FAMILY="unknown"
            fi
            ;;
    esac
    
    print_info "Distribution family: $DISTRO_FAMILY"
}

# Detect system architecture and multiarch path
detect_architecture() {
    ARCH=$(uname -m)
    
    case "$ARCH" in
        x86_64|amd64)
            ARCH_NAME="x86_64"
            MULTIARCH="x86_64-linux-gnu"
            ;;
        i?86)
            ARCH_NAME="i386"
            MULTIARCH="i386-linux-gnu"
            ;;
        aarch64|arm64)
            ARCH_NAME="aarch64"
            MULTIARCH="aarch64-linux-gnu"
            ;;
        armv7*|armhf)
            ARCH_NAME="armhf"
            MULTIARCH="arm-linux-gnueabihf"
            ;;
        *)
            print_error "Unsupported architecture: $ARCH"
            exit 1
            ;;
    esac
    
    print_info "System architecture: $ARCH_NAME (multiarch: $MULTIARCH)"
}

# Determine plugin installation paths based on distribution
determine_paths() {
    case "$DISTRO_FAMILY" in
        debian)
            # Debian/Ubuntu use multiarch paths
            PLUGIN_DIR="/usr/lib/$MULTIARCH/xfce4/panel/plugins"
            ;;
        redhat)
            # Fedora/RHEL typically use /usr/lib64 on x86_64
            if [ "$ARCH_NAME" = "x86_64" ]; then
                PLUGIN_DIR="/usr/lib64/xfce4/panel/plugins"
            else
                PLUGIN_DIR="/usr/lib/xfce4/panel/plugins"
            fi
            ;;
        arch)
            # Arch uses /usr/lib regardless of architecture
            PLUGIN_DIR="/usr/lib/xfce4/panel/plugins"
            ;;
        suse)
            # openSUSE similar to Fedora
            if [ "$ARCH_NAME" = "x86_64" ]; then
                PLUGIN_DIR="/usr/lib64/xfce4/panel/plugins"
            else
                PLUGIN_DIR="/usr/lib/xfce4/panel/plugins"
            fi
            ;;
        *)
            # Fallback: try to detect from existing plugins
            if [ -d "/usr/lib/$MULTIARCH/xfce4/panel/plugins" ]; then
                PLUGIN_DIR="/usr/lib/$MULTIARCH/xfce4/panel/plugins"
            elif [ -d "/usr/lib64/xfce4/panel/plugins" ]; then
                PLUGIN_DIR="/usr/lib64/xfce4/panel/plugins"
            elif [ -d "/usr/lib/xfce4/panel/plugins" ]; then
                PLUGIN_DIR="/usr/lib/xfce4/panel/plugins"
            else
                print_error "Cannot determine plugin directory"
                exit 1
            fi
            ;;
    esac
    
    # Common paths for all distributions
    DESKTOP_DIR="/usr/share/xfce4/panel/plugins"
    ICON_DIR="/usr/share/icons/hicolor"
    
    print_info "Plugin directory: $PLUGIN_DIR"
    print_info "Desktop file directory: $DESKTOP_DIR"
    print_info "Icon directory: $ICON_DIR"
}

# Check if the plugin is already built
check_build() {
    if [ ! -f "builddir/src/libapplemenu.so" ]; then
        print_error "Plugin not built. Please run ./build.sh first"
        exit 1
    fi
    
    print_success "Found built plugin"
}

# Install the plugin files
install_plugin() {
    print_info "Installing plugin files..."
    
    # Create directories if they don't exist
    mkdir -p "$PLUGIN_DIR"
    mkdir -p "$DESKTOP_DIR"
    mkdir -p "$ICON_DIR/48x48/apps"
    mkdir -p "$ICON_DIR/scalable/apps"
    
    # Install plugin library
    print_info "Installing plugin library..."
    cp -f "builddir/src/libapplemenu.so" "$PLUGIN_DIR/"
    chmod 755 "$PLUGIN_DIR/libapplemenu.so"
    
    # Install desktop file
    print_info "Installing desktop file..."
    cp -f "data/applemenu.desktop" "$DESKTOP_DIR/"
    chmod 644 "$DESKTOP_DIR/applemenu.desktop"
    
    # Install icons
    print_info "Installing icons..."
    if [ -f "data/icons/48x48/applemenu.png" ]; then
        cp -f "data/icons/48x48/applemenu.png" "$ICON_DIR/48x48/apps/"
        chmod 644 "$ICON_DIR/48x48/apps/applemenu.png"
    fi
    
    if [ -f "data/icons/scalable/applemenu.svg" ]; then
        cp -f "data/icons/scalable/applemenu.svg" "$ICON_DIR/scalable/apps/"
        chmod 644 "$ICON_DIR/scalable/apps/applemenu.svg"
    fi
    
    # Update icon cache
    if command -v gtk-update-icon-cache >/dev/null 2>&1; then
        print_info "Updating icon cache..."
        gtk-update-icon-cache -f -t "$ICON_DIR" 2>/dev/null || true
    fi
    
    print_success "Plugin installed successfully!"
}

# Restart XFCE panel
restart_panel() {
    print_info "Restarting XFCE panel..."
    
    # Try to restart panel for all users currently running it
    for pid in $(pgrep xfce4-panel); do
        user=$(ps -o user= -p $pid | tr -d ' ')
        if [ ! -z "$user" ]; then
            print_info "Restarting panel for user: $user"
            su - "$user" -c "xfce4-panel -r" 2>/dev/null || true
        fi
    done
    
    print_success "Panel restart completed"
}

# Main installation process
main() {
    echo "======================================"
    echo "XFCE Apple Menu Plugin Installer"
    echo "======================================"
    echo
    
    # Check root privileges
    check_root
    
    # Detect system
    detect_distro
    detect_architecture
    determine_paths
    
    # Check build
    check_build
    
    # Confirm installation
    echo
    print_warning "This will install the plugin to system directories:"
    echo "  - Plugin: $PLUGIN_DIR/libapplemenu.so"
    echo "  - Desktop: $DESKTOP_DIR/applemenu.desktop"
    echo "  - Icons: $ICON_DIR"
    echo
    read -p "Continue with installation? [y/N] " -n 1 -r
    echo
    
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_info "Installation cancelled"
        exit 0
    fi
    
    # Install
    install_plugin
    
    # Offer to restart panel
    echo
    read -p "Restart XFCE panel now? [y/N] " -n 1 -r
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        restart_panel
    else
        print_info "Please restart XFCE panel manually: xfce4-panel -r"
    fi
    
    echo
    print_success "Installation complete!"
    print_info "You can now add the Apple Menu to your panel through 'Panel Preferences' -> 'Items'"
}

# Run main function
main "$@"
