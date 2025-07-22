# Plugin sources
applemenu_sources = [
  'applemenu.c',
  'applemenu.h'
]

# Dependencies for the plugin
applemenu_deps = [
  glib_dep,
  gtk_dep,
  libxfce4panel_dep,
  libxfce4ui_dep,
  libxfce4util_dep,
]

if exo_dep.found()
  applemenu_deps += exo_dep
endif

if dbus_dep.found()
  applemenu_deps += dbus_dep
endif

# Build the plugin as a shared module
applemenu_lib = shared_module('applemenu',
  applemenu_sources,
  dependencies: applemenu_deps,
  include_directories: inc,
  install: true,
  install_dir: get_option('libdir') / 'xfce4' / 'panel' / 'plugins',
  name_prefix: 'lib',
  c_args: [
    '-DG_LOG_DOMAIN="xfce4-applemenu-plugin"',
  ]
)

# Desktop file
desktop_file = i18n.merge_file(
  input: 'applemenu.desktop.in',
  output: 'applemenu.desktop',
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: get_option('datadir') / 'xfce4' / 'panel' / 'plugins'
)
