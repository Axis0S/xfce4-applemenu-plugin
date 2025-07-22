project('xfce4-applemenu-plugin', 'c',
  version: '0.3.0',
  license: 'GPL-2.0-or-later',
  default_options: [
    'c_std=gnu11',
    'warning_level=2',
    'buildtype=debugoptimized',
  ],
  meson_version: '>= 0.49.0'
)

# Add project arguments
cc = meson.get_compiler('c')
add_project_arguments([
  '-DHAVE_CONFIG_H',
  '-DPACKAGE_LOCALE_DIR="@0@"'.format(get_option('prefix') / get_option('localedir')),
], language: 'c')

# Dependencies - Compatible with Debian 11
glib_dep = dependency('glib-2.0', version: '>= 2.66')
gtk_dep = dependency('gtk+-3.0', version: '>= 3.24')
libxfce4panel_dep = dependency('libxfce4panel-2.0', version: '>= 4.16')
libxfce4ui_dep = dependency('libxfce4ui-2', version: '>= 4.16')
libxfce4util_dep = dependency('libxfce4util-1.0', version: '>= 4.16')
exo_dep = dependency('exo-2', version: '>= 4.16', required: false)

# Optional dependencies
dbus_dep = dependency('dbus-glib-1', version: '>= 0.110', required: get_option('dbus'))

# Create config.h
config_h = configuration_data()
config_h.set_quoted('PACKAGE_NAME', meson.project_name())
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', meson.project_name())
config_h.set('ENABLE_NLS', true)

# Optional features
if dbus_dep.found()
  config_h.set('HAVE_DBUS', 1)
endif

configure_file(
  output: 'config.h',
  configuration: config_h
)

# Include directories
inc = include_directories('.')

# i18n
i18n = import('i18n')

# Subdirectories
subdir('src')
subdir('data')
subdir('po')

# Summary
summary({
  'prefix': get_option('prefix'),
  'libdir': get_option('prefix') / get_option('libdir'),
  'datadir': get_option('prefix') / get_option('datadir'),
}, section: 'Directories')

summary({
  'D-Bus support': dbus_dep.found(),
}, section: 'Features')
