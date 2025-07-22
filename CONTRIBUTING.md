# Contributing to xfce4-applemenu-plugin

Thank you for your interest in contributing to xfce4-applemenu-plugin! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Submitting Contributions](#submitting-contributions)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Reporting Issues](#reporting-issues)

## Code of Conduct

This project follows the [Xfce Code of Conduct](https://xfce.org/about/code-of-conduct). Please read and follow it in all your interactions with the project.

## Getting Started

### Prerequisites

Before contributing, ensure you have:

- Git installed on your system
- A GitHub account (or GitLab if the project is hosted there)
- Basic knowledge of C programming
- Familiarity with GTK+ and Xfce panel plugin development (helpful but not required)

### Fork and Clone

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/xfce4-applemenu-plugin.git
   cd xfce4-applemenu-plugin
   ```
3. Add the upstream repository:
   ```bash
   git remote add upstream https://github.com/ORIGINAL_OWNER/xfce4-applemenu-plugin.git
   ```

## Development Setup

### Dependencies

Install the required dependencies:

```bash
# On Debian/Ubuntu
sudo apt-get install build-essential meson ninja-build \
    libxfce4panel-2.0-dev libxfce4ui-2-dev libxfce4util-dev \
    libgtk-3-dev libglib2.0-dev libvala-dev valac \
    libwnck-3-dev libbamf3-dev libdbusmenu-glib-dev \
    libdbusmenu-gtk3-dev unity-gtk-module-common \
    appmenu-gtk-module-common

# On Fedora
sudo dnf install meson ninja-build gcc vala \
    xfce4-panel-devel libxfce4ui-devel libxfce4util-devel \
    gtk3-devel glib2-devel libwnck3-devel bamf-devel \
    libdbusmenu-devel libdbusmenu-gtk3-devel
```

### Building the Project

1. Create a build directory:
   ```bash
   meson setup build
   ```

2. Compile the project:
   ```bash
   ninja -C build
   ```

3. Install locally for testing:
   ```bash
   sudo ninja -C build install
   ```

4. To uninstall:
   ```bash
   sudo ninja -C build uninstall
   ```

## Making Changes

### Branch Naming

Create a new branch for your changes:

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-description
```

Use descriptive branch names:
- `feature/` for new features
- `fix/` for bug fixes
- `docs/` for documentation changes
- `refactor/` for code refactoring

### Commit Messages

Follow these commit message guidelines:

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests liberally after the first line

Example:
```
Add support for custom menu icons

- Implement icon loading from theme
- Add configuration option for icon size
- Update UI to display custom icons

Fixes #123
```

## Submitting Contributions

### Pull Request Process

1. Update your fork with the latest upstream changes:
   ```bash
   git fetch upstream
   git checkout main
   git merge upstream/main
   ```

2. Rebase your feature branch:
   ```bash
   git checkout feature/your-feature-name
   git rebase main
   ```

3. Push your changes:
   ```bash
   git push origin feature/your-feature-name
   ```

4. Create a Pull Request on GitHub with:
   - A clear title and description
   - Reference to any related issues
   - Screenshots for UI changes
   - Test results if applicable

### Pull Request Checklist

Before submitting, ensure:

- [ ] Code compiles without warnings
- [ ] All tests pass (if applicable)
- [ ] Code follows the project's coding standards
- [ ] Documentation is updated if needed
- [ ] Commit messages are clear and descriptive
- [ ] Changes are rebased on the latest main branch

## Coding Standards

### C Code Style

Follow the Xfce coding style:

```c
/* Function declarations */
static void
function_name (GtkWidget *widget,
               gpointer   user_data)
{
    /* Variable declarations at the beginning */
    gint         i;
    gchar       *string;
    GtkWidget   *button;

    /* Code implementation */
    for (i = 0; i < 10; i++)
    {
        /* Use brackets even for single statements */
        do_something ();
    }

    /* Free allocated resources */
    g_free (string);
}
```

Key points:
- Use 4 spaces for indentation (no tabs)
- Place opening braces on new lines
- Use spaces around operators and after commas
- Align function parameters
- Use descriptive variable names
- Comment complex logic

### Vala Code Style

For Vala code, follow similar conventions:

```vala
public class MenuWidget : Gtk.Box {
    private Gtk.Label label;
    
    public MenuWidget () {
        Object (orientation: Gtk.Orientation.HORIZONTAL);
    }
    
    construct {
        label = new Gtk.Label ("Menu");
        add (label);
    }
    
    public void update_label (string text) {
        label.set_text (text);
    }
}
```

## Testing

### Manual Testing

1. Test the plugin with different Xfce panel configurations:
   - Horizontal and vertical panels
   - Different panel sizes
   - Multiple monitors

2. Test with various applications:
   - GTK3 applications
   - GTK2 applications (if supported)
   - Qt applications with appmenu support
   - LibreOffice

3. Test edge cases:
   - No applications running
   - Applications without menu support
   - Rapid application switching

### Automated Testing

If the project includes tests:

```bash
meson test -C build
```

Add new tests for new features when possible.

## Documentation

### Code Documentation

- Add comments for complex logic
- Document function parameters and return values
- Use Doxygen-style comments for public APIs:

```c
/**
 * applemenu_get_active_window:
 * @panel: The XfcePanelPlugin instance
 * 
 * Gets the currently active window.
 * 
 * Returns: (transfer none): The active window or NULL
 */
static GtkWindow *
applemenu_get_active_window (XfcePanelPlugin *panel)
{
    /* Implementation */
}
```

### User Documentation

Update relevant documentation:
- README.md for major features
- Help documentation if available
- Configuration examples

## Reporting Issues

When reporting issues, please include:

1. **Description**: Clear description of the problem
2. **Steps to Reproduce**: Detailed steps to reproduce the issue
3. **Expected Behavior**: What you expected to happen
4. **Actual Behavior**: What actually happened
5. **System Information**:
   - Xfce version
   - Distribution and version
   - GTK version
   - Panel configuration
6. **Screenshots**: If applicable
7. **Error Messages**: Any relevant error messages or logs

### Getting Help

If you need help:

- Check existing issues and pull requests
- Read the documentation
- Ask in the Xfce development channels
- Contact the maintainers

## License

By contributing to xfce4-applemenu-plugin, you agree that your contributions will be licensed under the same license as the project (check LICENSE file).

## Recognition

Contributors will be recognized in:
- The project's AUTHORS file
- Release notes for significant contributions
- Project documentation

Thank you for contributing to xfce4-applemenu-plugin!
