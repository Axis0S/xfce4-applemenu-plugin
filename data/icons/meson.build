# Icon installation
icon_sizes = ['16x16', '22x22', '24x24']

foreach size : icon_sizes
  install_data(
    size / 'apple-logo.svg',
    install_dir: get_option('datadir') / 'icons' / 'hicolor' / size / 'apps'
  )
endforeach

# Update icon cache after installation
meson.add_install_script('gtk-update-icon-cache',
  get_option('datadir') / 'icons' / 'hicolor',
  skip_if_destdir: true
)
