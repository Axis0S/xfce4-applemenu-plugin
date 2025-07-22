# XFCE4 Apple Menu Plugin

A panel plugin for XFCE4 that mimics the macOS Apple menu, providing a familiar and intuitive system menu experience for XFCE desktop users.

![XFCE4 Apple Menu Plugin]

## Features

- 🍎 **Apple-style Menu**: Familiar dropdown menu with Apple logo
- 💻 **System Information**: Quick access to system details
- 📦 **Package Management**: Integration with pamac-manager for software management
- 📄 **Recent Items**: Track and access recently used files and applications
- ⚡ **Quick Actions**: Sleep, restart, shutdown, lock screen, and log out
- 🎨 **Themeable**: Supports GTK+ theming and custom icons
- 🌍 **Internationalization**: Multi-language support

## Menu Structure

- **About This Computer** - System information and details
- **System Preferences** - Quick access to XFCE Settings Manager
- **App Store** - Launch pamac-manager for package management
- **Recent Items** - Recently accessed documents and applications
- **Force Quit** - Application management
- **System Actions** - Sleep, Restart, Shut Down, Lock Screen, Log Out

## Requirements

### Runtime Dependencies
- XFCE4 Panel (>= 4.16)
- GTK+3 (>= 3.24)
- libxfce4util (>= 4.16)
- libxfce4ui (>= 4.16)
- pamac-manager (for App Store functionality, optional)

### Build Dependencies
- meson (>= 0.49.0)
- ninja-build
- gcc or clang
- pkg-config
- libxfce4panel-2.0-dev (>= 4.16)
- libxfce4ui-2-dev (>= 4.16)
- libxfce4util-dev (>= 4.16)
- libgtk-3-dev (>= 3.24)
- libglib2.0-dev (>= 2.66)
- libexo-2-dev (>= 4.16, optional)
- libdbus-glib-1-dev (>= 0.110, optional)

### Installing Build Dependencies on Debian 11

```bash
sudo apt update
sudo apt install meson ninja-build gcc pkg-config \
    libxfce4panel-2.0-dev libxfce4ui-2-dev \
    libxfce4util-dev libgtk-3-dev libglib2.0-dev \
    libexo-2-dev libdbus-glib-1-dev
```

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/Axis0S/xfce4-applemenu-plugin.git
cd xfce4-applemenu-plugin

# Build and install using Meson
meson setup build --prefix=/usr
cd build
ninja
sudo ninja install

# Or use the provided build script
./build.sh
cd build
sudo ninja install

# Restart XFCE Panel
xfce4-panel -r
```

### Arch Linux (AUR)

```bash
# Using yay
yay -S xfce4-applemenu-plugin

# Or using pamac
pamac install xfce4-applemenu-plugin
```

## Configuration

Right-click on the plugin in the panel and select "Properties" to access:

- Icon selection (Apple logo or custom)
- Recent items settings
- Menu behavior options
- Keyboard shortcuts

## Pamac Integration

The "App Store" menu item is configured to launch pamac-manager, providing:
- Software installation and removal
- System updates
- AUR support (if enabled)
- Package search and categories

To ensure pamac-manager is installed:
```bash
sudo pacman -S pamac-gtk
```

## Theming

The plugin respects your GTK+ theme. For custom styling, add to `~/.config/gtk-3.0/gtk.css`:

```css
#xfce4-applemenu-plugin button {
    padding: 4px 8px;
    border-radius: 4px;
}

#xfce4-applemenu-plugin button:hover {
    background-color: @theme_selected_bg_color;
}
```

## Troubleshooting

### Plugin not appearing
1. Ensure the plugin is properly installed: `ls /usr/lib/xfce4/panel/plugins/`
2. Add it through Panel Preferences → Items → Add → Apple Menu

### Pamac not launching
1. Verify pamac-manager is installed: `which pamac-manager`
2. Check if pamac service is running: `systemctl status pamac-daemon`

### Icons not displaying
1. Install an icon theme with Apple logo support
2. Or place custom Apple logo in `~/.local/share/icons/`

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Follow XFCE coding standards
4. Submit a pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## License

This project is licensed under the GPL-2.0+ License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- XFCE Development Team for the panel plugin framework
- Apple Inc. for the menu design inspiration
- Pamac developers for the excellent package manager
- Contributors and translators

## Links

- [Documentation](DOCUMENT.md)
- [XFCE Panel Plugin Guide](https://docs.xfce.org/xfce/xfce4-panel/theming)
- [Report Issues](https://github.com/Axis0S/xfce4-applemenu-plugin/issues)

## Author

Kamil 'Novik' Nowicki
- Email: novik@axisos.org
- Website: https://axisos.org
- GitHub: https://github.com/Axis0S
