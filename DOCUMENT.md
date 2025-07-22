# XFCE4 Apple Menu Plugin Documentation

## Overview

The XFCE4 Apple Menu Plugin is a panel plugin that mimics the macOS top bar main menu functionality, providing a unified system menu with the iconic Apple logo as its launcher. This plugin brings the familiar macOS-style menu experience to XFCE desktop environments.

## Features

### Core Functionality
- **Apple Logo Button**: Displays the Apple logo (or custom icon) as the main menu launcher
- **Dropdown Menu System**: Provides a comprehensive dropdown menu similar to macOS
- **System Integration**: Deep integration with XFCE4 panel and system services

### Menu Items
The plugin replicates the standard macOS Apple menu structure:

1. **About This Computer**
   - Shows system information
   - OS version, kernel, hardware specs
   - Links to system settings

2. **System Preferences**
   - Quick access to XFCE Settings Manager
   - Categorized settings shortcuts

3. **App Store** (Optional)
   - Links to pamac-manager (Arch/Manjaro package manager GUI)
   - Software center integration
   - Quick access to system updates

4. **Recent Items**
   - Recently opened documents
   - Recently used applications
   - Clear recent items option

5. **Force Quit**
   - Application force quit dialog
   - Process management interface

6. **Sleep/Wake Options**
   - Sleep
   - Restart
   - Shut Down
   - Lock Screen
   - Log Out

## Technical Architecture

### Plugin Structure
```
xfce-applemenu/
├── src/
│   ├── applemenu.c          # Main plugin code
│   ├── applemenu.h          # Header definitions
│   ├── menu-builder.c       # Menu construction logic
│   ├── menu-builder.h
│   ├── system-info.c        # System information gathering
│   ├── system-info.h
│   ├── recent-items.c       # Recent items tracking
│   └── recent-items.h
├── data/
│   ├── icons/               # Apple logo and other icons
│   ├── applemenu.desktop    # Desktop entry file
│   └── applemenu.xml        # Menu configuration
├── po/                      # Translations
├── meson.build             # Meson build configuration
├── meson_options.txt       # Build options
├── README.md               # Basic readme
└── DOCUMENT.md             # This documentation
```

### Dependencies
- **XFCE4 Panel** (>= 4.16)
- **GTK+3** (>= 3.24)
- **libxfce4util** (>= 4.16)
- **libxfce4ui** (>= 4.16)
- **GLib** (>= 2.66)
- **Exo** (>= 4.16, optional)
- **D-Bus GLib** (>= 0.110, optional)

### Build Requirements
- **Meson** (>= 0.49.0)
- **Ninja** (>= 1.8.2)
- **GCC** (>= 8.3.0) or **Clang**
- **pkg-config**

## Implementation Details

### Panel Integration
The plugin registers with the XFCE4 panel using the standard plugin API:

```c
XFCE_PANEL_PLUGIN_REGISTER(applemenu_construct);
```

### Menu Construction
The menu is built dynamically using GTK+ menu widgets:
- **GtkMenu** for the main dropdown
- **GtkMenuItem** for individual items
- **GtkSeparatorMenuItem** for visual separation
- **GtkImageMenuItem** for items with icons

### Icon Handling
- Supports SVG and PNG formats for the Apple logo
- Automatic scaling based on panel size
- Fallback to generic icon if Apple logo not available
- Respects system icon theme

### Recent Items Tracking
- Monitors GTK+ recent manager
- Integrates with XFCE4 recent documents
- Configurable item limit
- Grouped by type (Documents, Applications, etc.)

### System Information
Gathered using various system calls and libraries:
- **uname()** for kernel information
- **/proc/cpuinfo** for processor details
- **/proc/meminfo** for memory statistics
- **D-Bus** for session information

## Configuration

### User Settings
Stored in `~/.config/xfce4/panel/applemenu-{id}.rc`:

```ini
[Configuration]
show_recent_items=true
recent_items_max=10
show_system_info=true
use_apple_logo=true
custom_icon_path=
menu_width=250
show_descriptions=true
```

### Panel Properties
Configurable through XFCE4 Panel preferences:
- Icon size (follows panel size)
- Position on panel
- Expand behavior
- Show label option

## Styling

### CSS Theming
The plugin supports GTK+ CSS theming:

```css
#xfce4-applemenu-plugin {
    background-color: transparent;
    border: none;
}

#xfce4-applemenu-plugin button {
    padding: 2px 8px;
    border-radius: 4px;
}

#xfce4-applemenu-plugin button:hover {
    background-color: @selected_bg_color;
}

.applemenu-dropdown {
    border-radius: 6px;
    box-shadow: 0 2px 8px rgba(0,0,0,0.2);
}
```

### Icon Themes
Compatible with standard icon naming specification:
- `apple-logo` (preferred)
- `distributor-logo` (fallback)
- `applications-system` (final fallback)

## Development

### Building from Source
```bash
# Using Meson build system
meson setup build --prefix=/usr
cd build
ninja
sudo ninja install

# Or use the provided build script
./build.sh
cd build
sudo ninja install
```

### Debug Mode
Enable debug output:
```bash
PANEL_DEBUG=1 xfce4-panel
```

### Contributing
1. Follow XFCE coding standards
2. Use GLib/GTK+ conventions
3. Ensure i18n support for all user-visible strings
4. Test with multiple panel configurations

## Localization

### String Extraction
```bash
cd po/
make update-po
```

### Adding Translations
1. Create new .po file in po/ directory
2. Add language code to po/LINGUAS
3. Translate strings
4. Test with `LANG=xx_XX xfce4-panel`

## Known Issues

### Compatibility
- Some menu items may not map perfectly to Linux equivalents
- Force Quit functionality differs from macOS implementation
- System information format varies by distribution

### Visual Differences
- GTK+ theming limitations prevent exact macOS appearance
- Menu animations differ from native macOS behavior
- Shadow and blur effects depend on compositor

## Future Enhancements

### Planned Features
1. **Spotlight Search Integration**
   - Quick search functionality
   - Application launcher
   - File search

2. **Enhanced Recent Items**
   - Favorite items
   - Pinned applications
   - Smart folders

3. **System Monitoring**
   - CPU/Memory graphs
   - Network status
   - Battery information

4. **Notification Center**
   - Notification history
   - Do Not Disturb mode
   - Quick actions

### Customization Options
- Multiple icon styles (Classic, Modern, Custom)
- Menu layout templates
- Keyboard shortcuts
- Plugin actions

## License

This plugin is licensed under the GNU General Public License version 2 or later (GPL-2.0+), consistent with other XFCE4 panel plugins.

## Support

### Bug Reports
File issues at: https://github.com/Axis0S/xfce4-applemenu-plugin

### Documentation
- This document
- Man page: `man xfce4-applemenu-plugin`
- XFCE Wiki: https://wiki.xfce.org

### Community
- XFCE Forums
- IRC: #xfce on Libera.Chat
- Mailing lists

## References

- [XFCE Panel Plugin Development](https://docs.xfce.org/xfce/xfce4-panel/theming)
- [GTK+ 3 Reference](https://developer.gnome.org/gtk3/stable/)
- [macOS Human Interface Guidelines](https://developer.apple.com/design/human-interface-guidelines/)
