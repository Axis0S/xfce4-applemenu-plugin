# Changelog

All notable changes to the xfce-applemenu plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2025-01-22

### Fixed
- Fixed issue where clicking the Apple logo a second time would not close the menu
- Improved menu visibility state tracking to properly handle open/close cycles
- Fixed menu event handling to ensure proper synchronization between button and menu states
- Changed menu popup method from `gtk_menu_popup_at_widget()` to `gtk_menu_popup()` for better compatibility

### Changed
- **Migrated build system from Autotools to Meson** for faster and cleaner builds
- Replaced intltool with native Meson i18n support
- Simplified menu show/hide callbacks by adding explicit `gtk_widget_hide()` calls
- Improved code clarity in menu visibility management

### Technical Details
- Build system now uses Meson >= 0.49.0 instead of Autotools
- Menu popup now uses `gtk_menu_popup()` with proper positioning function
- Added `gtk_widget_hide()` calls in menu hide/deactivate callbacks for explicit menu closing
- Menu visibility is tracked through the `menu_visible` flag updated by menu show/hide/deactivate callbacks

### Metadata
- Updated all copyright headers to Kamil 'Novik' Nowicki <novik@axisos.org>
- Added project website: https://axisos.org
- Updated repository URL: https://github.com/Axis0S/xfce4-applemenu-plugin

## [0.1.0] - 2025-01-22

### Added
- Initial release of xfce-applemenu plugin
- Apple-style menu for XFCE panel with macOS-inspired design
- Menu items include:
  - About This Computer (shows system information)
  - System Preferences (launches xfce4-settings-manager)
  - App Store (configurable, defaults to pamac-manager)
  - Recent Items (placeholder, disabled)
  - Force Quit (launches xkill)
  - Sleep (system suspend)
  - Restart (system reboot)
  - Shut Down (system halt)
  - Lock Screen (xflock4)
  - Log Out (with current username display)
- Configuration dialog with options for:
  - Custom icon selection
  - App Store command customization
  - Menu transparency adjustment
  - Show/hide recent items option
- Support for Apple logo icon with automatic fallback to distributor logo
- Automatic icon sizing based on panel size
- Panel orientation support (horizontal/vertical)
- Configuration persistence using XfceRc
- Internationalization support (i18n ready)
- GTK+ 3 based implementation
