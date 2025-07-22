/*
 * Copyright (C) 2024-2025 Kamil 'Novik' Nowicki <novik@axisos.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>
#include <exo/exo.h>
#include <dbus/dbus-glib.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "applemenu.h"

/* Plugin structure */
typedef struct {
    XfcePanelPlugin *plugin;
    GtkWidget       *button;
    GtkWidget       *icon;
    GtkWidget       *menu;
    gboolean         menu_visible;  /* Track menu visibility state */
    
    /* Configuration */
    gboolean         show_recent_items;
    gint             recent_items_max;
    gboolean         use_apple_logo;
    gchar           *custom_icon_name;
    gchar           *app_store_command;
    gint             transparency;
} AppleMenuPlugin;

/* Prototypes */
static void applemenu_construct(XfcePanelPlugin *plugin);
static void applemenu_free_data(XfcePanelPlugin *plugin, AppleMenuPlugin *applemenu);
static void applemenu_position_menu(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer data);
static void applemenu_button_clicked(GtkWidget *button, AppleMenuPlugin *applemenu);
static void applemenu_create_menu(AppleMenuPlugin *applemenu);
static gboolean applemenu_size_changed(XfcePanelPlugin *plugin, guint size, AppleMenuPlugin *applemenu);
static void applemenu_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, AppleMenuPlugin *applemenu);
static void applemenu_configure_plugin(XfcePanelPlugin *plugin, AppleMenuPlugin *applemenu);
static void applemenu_save_config(XfcePanelPlugin *plugin, AppleMenuPlugin *applemenu);
static void applemenu_load_config(AppleMenuPlugin *applemenu);

/* Menu callbacks */
static void applemenu_about_computer(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_system_preferences(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_app_store(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_force_quit(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_sleep(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_restart(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_shutdown(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_lock_screen(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_logout(GtkMenuItem *item G_GNUC_UNUSED, gpointer data);
static void applemenu_on_menu_show(GtkWidget *menu, gpointer data);
static void applemenu_on_menu_hide(GtkWidget *menu, gpointer data);
static void applemenu_on_menu_deactivate(GtkWidget *menu, gpointer data);

/* Register the plugin */
XFCE_PANEL_PLUGIN_REGISTER(applemenu_construct);

/* Plugin construction */
static void
applemenu_construct(XfcePanelPlugin *plugin)
{
    AppleMenuPlugin *applemenu;
    GtkWidget *icon;
    
    /* Allocate plugin structure */
    applemenu = g_slice_new0(AppleMenuPlugin);
    applemenu->plugin = plugin;
    
    /* Initialize configuration with defaults */
    applemenu->show_recent_items = TRUE;
    applemenu->recent_items_max = 10;
    applemenu->use_apple_logo = TRUE;
    applemenu->custom_icon_name = g_strdup(APPLEMENU_ICON_NAME);
    applemenu->app_store_command = g_strdup(DEFAULT_APP_STORE_COMMAND);
    applemenu->transparency = DEFAULT_TRANSPARENCY;
    applemenu->menu_visible = FALSE;
    
    /* Create button */
    applemenu->button = xfce_panel_create_button();
    gtk_widget_set_tooltip_text(applemenu->button, _("Apple Menu"));
    gtk_button_set_relief(GTK_BUTTON(applemenu->button), GTK_RELIEF_NONE);
    
    /* Create icon */
    icon = gtk_image_new_from_icon_name(APPLEMENU_ICON_NAME, GTK_ICON_SIZE_BUTTON);
    if (!gtk_image_get_pixbuf(GTK_IMAGE(icon))) {
        /* Fallback to distributor logo if Apple logo not found */
        gtk_image_set_from_icon_name(GTK_IMAGE(icon), APPLEMENU_FALLBACK_ICON, GTK_ICON_SIZE_BUTTON);
    }
    applemenu->icon = icon;
    gtk_container_add(GTK_CONTAINER(applemenu->button), icon);
    gtk_widget_show(icon);
    
    /* Connect button signal */
    g_signal_connect(G_OBJECT(applemenu->button), "clicked",
                     G_CALLBACK(applemenu_button_clicked), applemenu);
    
    /* Add button to plugin */
    gtk_container_add(GTK_CONTAINER(plugin), applemenu->button);
    gtk_widget_show(applemenu->button);
    
    /* Load configuration */
    applemenu_load_config(applemenu);
    
    /* Connect plugin signals */
    g_signal_connect(G_OBJECT(plugin), "free-data",
                     G_CALLBACK(applemenu_free_data), applemenu);
    g_signal_connect(G_OBJECT(plugin), "save",
                     G_CALLBACK(applemenu_save_config), applemenu);
    g_signal_connect(G_OBJECT(plugin), "size-changed",
                     G_CALLBACK(applemenu_size_changed), applemenu);
    g_signal_connect(G_OBJECT(plugin), "orientation-changed",
                     G_CALLBACK(applemenu_orientation_changed), applemenu);
    g_signal_connect(G_OBJECT(plugin), "configure-plugin",
                     G_CALLBACK(applemenu_configure_plugin), applemenu);
    
    /* Create menu */
    applemenu_create_menu(applemenu);
    
    /* Show properties dialog on first use */
    xfce_panel_plugin_menu_show_configure(plugin);
    
    /* Store plugin data */
    g_object_set_data(G_OBJECT(plugin), "applemenu-data", applemenu);
}

/* Free plugin data */
static void
applemenu_free_data(XfcePanelPlugin *plugin G_GNUC_UNUSED, AppleMenuPlugin *applemenu)
{
    /* Destroy menu */
    if (applemenu->menu)
        gtk_widget_destroy(applemenu->menu);
    
    /* Free configuration */
    g_free(applemenu->custom_icon_name);
    g_free(applemenu->app_store_command);
    
    /* Free plugin structure */
    g_slice_free(AppleMenuPlugin, applemenu);
}

/* Position menu */
static void
applemenu_position_menu(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer data)
{
    AppleMenuPlugin *applemenu = (AppleMenuPlugin *)data;
    xfce_panel_plugin_position_menu(menu, x, y, push_in, applemenu->plugin);
}

/* Handle button click */
static void
applemenu_button_clicked(GtkWidget *button G_GNUC_UNUSED, AppleMenuPlugin *applemenu)
{
    /* Toggle menu visibility */
    if (applemenu->menu_visible) {
        /* Menu is visible, close it */
        gtk_menu_popdown(GTK_MENU(applemenu->menu));
        gtk_widget_hide(GTK_WIDGET(applemenu->menu));
        applemenu->menu_visible = FALSE;
    } else {
        /* Menu is not visible, show it */
        gtk_menu_popup_at_widget(GTK_MENU(applemenu->menu),
                                 applemenu->button,
                                 GDK_GRAVITY_SOUTH_WEST,
                                 GDK_GRAVITY_NORTH_WEST,
                                 NULL);
    }
}

/* Create menu */
static void
applemenu_create_menu(AppleMenuPlugin *applemenu)
{
    GtkWidget *menu, *item, *image;
    
    /* Create menu */
    menu = gtk_menu_new();
    applemenu->menu = menu;
    
    /* Connect menu signals to track visibility and sync with button */
    g_signal_connect(G_OBJECT(menu), "show", 
                     G_CALLBACK(applemenu_on_menu_show), applemenu);
    g_signal_connect(G_OBJECT(menu), "hide", 
                     G_CALLBACK(applemenu_on_menu_hide), applemenu);
    g_signal_connect(G_OBJECT(menu), "deactivate",
                     G_CALLBACK(applemenu_on_menu_deactivate), applemenu);
    
    /* About This Computer */
    item = gtk_image_menu_item_new_with_mnemonic(_("_About This Computer"));
    image = gtk_image_new_from_icon_name("computer", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_about_computer), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Separator */
    item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* System Preferences */
    item = gtk_image_menu_item_new_with_mnemonic(_("System _Preferences..."));
    image = gtk_image_new_from_icon_name("preferences-system", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_system_preferences), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* App Store (Pamac) */
    item = gtk_image_menu_item_new_with_mnemonic(_("_App Store..."));
    image = gtk_image_new_from_icon_name("system-software-install", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_app_store), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Separator */
    item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Recent Items (TODO: Implement submenu) */
    if (applemenu->show_recent_items) {
        item = gtk_image_menu_item_new_with_mnemonic(_("Recent _Items"));
        image = gtk_image_new_from_icon_name("document-open-recent", GTK_ICON_SIZE_MENU);
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
        gtk_widget_set_sensitive(item, FALSE); /* Disabled for now */
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
        gtk_widget_show(item);
        
        /* Separator */
        item = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
        gtk_widget_show(item);
    }
    
    /* Force Quit */
    item = gtk_image_menu_item_new_with_mnemonic(_("_Force Quit..."));
    image = gtk_image_new_from_icon_name("process-stop", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_force_quit), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Separator */
    item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Sleep */
    item = gtk_image_menu_item_new_with_mnemonic(_("_Sleep"));
    image = gtk_image_new_from_icon_name("system-suspend", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_sleep), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Restart */
    item = gtk_image_menu_item_new_with_mnemonic(_("_Restart..."));
    image = gtk_image_new_from_icon_name("system-reboot", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_restart), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Shut Down */
    item = gtk_image_menu_item_new_with_mnemonic(_("Shut _Down..."));
    image = gtk_image_new_from_icon_name("system-shutdown", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_shutdown), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Separator */
    item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Lock Screen */
    item = gtk_image_menu_item_new_with_mnemonic(_("_Lock Screen"));
    image = gtk_image_new_from_icon_name("system-lock-screen", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_lock_screen), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Log Out - Show current username */
    const gchar *username = g_get_user_name();
    gchar *logout_label = g_strdup_printf(_("Log Out %s..."), username);
    item = gtk_image_menu_item_new_with_mnemonic(logout_label);
    g_free(logout_label);
    image = gtk_image_new_from_icon_name("system-log-out", GTK_ICON_SIZE_MENU);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
    g_signal_connect(G_OBJECT(item), "activate",
                     G_CALLBACK(applemenu_logout), applemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    
    /* Apply transparency if set */
    if (applemenu->transparency < 100) {
        gtk_widget_set_opacity(GTK_WIDGET(menu), 
                              applemenu->transparency / 100.0);
    }
    
    gtk_widget_show_all(menu);
}

/* Size changed callback */
static gboolean
applemenu_size_changed(XfcePanelPlugin *plugin G_GNUC_UNUSED, guint size, AppleMenuPlugin *applemenu)
{
    GtkIconSize icon_size;
    gint icon_width;
    
    /* Determine icon size based on panel size */
    if (size <= 16)
        icon_size = GTK_ICON_SIZE_MENU;
    else if (size <= 22)
        icon_size = GTK_ICON_SIZE_SMALL_TOOLBAR;
    else if (size <= 24)
        icon_size = GTK_ICON_SIZE_LARGE_TOOLBAR;
    else if (size <= 32)
        icon_size = GTK_ICON_SIZE_DND;
    else
        icon_size = GTK_ICON_SIZE_DIALOG;
    
    /* Update icon */
    gtk_image_set_from_icon_name(GTK_IMAGE(applemenu->icon),
                                 applemenu->use_apple_logo ? APPLEMENU_ICON_NAME : APPLEMENU_FALLBACK_ICON,
                                 icon_size);
    
    /* Set button size */
    gtk_widget_set_size_request(GTK_WIDGET(applemenu->button), size, size);
    
    return TRUE;
}

/* Orientation changed callback */
static void
applemenu_orientation_changed(XfcePanelPlugin *plugin G_GNUC_UNUSED, GtkOrientation orientation, AppleMenuPlugin *applemenu)
{
    /* Update button orientation if needed */
    gtk_orientable_set_orientation(GTK_ORIENTABLE(applemenu->button), orientation);
}

/* Menu show callback */
static void
applemenu_on_menu_show(GtkWidget *menu G_GNUC_UNUSED, gpointer data)
{
    AppleMenuPlugin *applemenu = (AppleMenuPlugin *)data;
    applemenu->menu_visible = TRUE;
}

/* Menu hide callback */
static void
applemenu_on_menu_hide(GtkWidget *menu G_GNUC_UNUSED, gpointer data)
{
    AppleMenuPlugin *applemenu = (AppleMenuPlugin *)data;
    applemenu->menu_visible = FALSE;
    gtk_widget_hide(menu);
}

/* Menu deactivate callback */
static void
applemenu_on_menu_deactivate(GtkWidget *menu G_GNUC_UNUSED, gpointer data)
{
    AppleMenuPlugin *applemenu = (AppleMenuPlugin *)data;
    applemenu->menu_visible = FALSE;
    gtk_widget_hide(menu);
}

/* Menu callbacks implementation */
static void
applemenu_about_computer(GtkMenuItem *item G_GNUC_UNUSED, gpointer data)
{
    AppleMenuPlugin *applemenu = (AppleMenuPlugin *)data;
    GtkWidget *dialog, *content, *label;
    gchar *text;
    struct utsname un;
    
    /* Get system information */
    uname(&un);
    
    /* Create dialog */
    dialog = gtk_dialog_new_with_buttons(_("About This Computer"),
                                         NULL,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         _("_Close"), GTK_RESPONSE_CLOSE,
                                         NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    
    /* Create content */
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 10);
    
    /* System information */
    text = g_strdup_printf("<b>%s</b>\n\n"
                          "<b>Operating System:</b> %s\n"
                          "<b>Kernel:</b> %s %s\n"
                          "<b>Architecture:</b> %s\n"
                          "<b>Hostname:</b> %s\n",
                          _("System Information"),
                          un.sysname, un.release, un.version,
                          un.machine, un.nodename);
    
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_box_pack_start(GTK_BOX(content), label, FALSE, FALSE, 0);
    
    g_free(text);
    
    /* Show dialog */
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void
applemenu_system_preferences(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Launch XFCE Settings Manager */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xfce4-settings-manager", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to open System Preferences"));
        g_error_free(error);
    }
}

static void
applemenu_app_store(GtkMenuItem *item G_GNUC_UNUSED, gpointer data)
{
    AppleMenuPlugin *applemenu = (AppleMenuPlugin *)data;
    GError *error = NULL;
    
    /* Launch configured app store command */
    if (!g_spawn_command_line_async(applemenu->app_store_command, &error)) {
        gchar *message = g_strdup_printf(_("Failed to open App Store (%s)"), applemenu->app_store_command);
        xfce_dialog_show_error(NULL, error, "%s", message);
        g_free(message);
        g_error_free(error);
    }
}

static void
applemenu_force_quit(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Launch xkill */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xkill", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to launch Force Quit"));
        g_error_free(error);
    }
}

static void
applemenu_sleep(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Suspend system */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xfce4-session-logout --suspend", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to suspend system"));
        g_error_free(error);
    }
}

static void
applemenu_restart(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Restart system */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xfce4-session-logout --reboot", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to restart system"));
        g_error_free(error);
    }
}

static void
applemenu_shutdown(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Shutdown system */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xfce4-session-logout --halt", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to shutdown system"));
        g_error_free(error);
    }
}

static void
applemenu_lock_screen(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Lock screen */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xflock4", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to lock screen"));
        g_error_free(error);
    }
}

static void
applemenu_logout(GtkMenuItem *item G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
    /* Log out */
    GError *error = NULL;
    if (!g_spawn_command_line_async("xfce4-session-logout", &error)) {
        xfce_dialog_show_error(NULL, error, _("Failed to log out"));
        g_error_free(error);
    }
}

/* Configuration loading */
static void
applemenu_load_config(AppleMenuPlugin *applemenu)
{
    gchar *file;
    XfceRc *rc;
    
    /* Get config file location */
    file = xfce_panel_plugin_save_location(applemenu->plugin, TRUE);
    if (G_UNLIKELY(!file))
        return;
    
    /* Open config file */
    rc = xfce_rc_simple_open(file, TRUE);
    g_free(file);
    
    if (G_UNLIKELY(!rc))
        return;
    
    /* Read settings */
    applemenu->show_recent_items = xfce_rc_read_bool_entry(rc, "show-recent-items", TRUE);
    applemenu->recent_items_max = xfce_rc_read_int_entry(rc, "recent-items-max", 10);
    
    g_free(applemenu->custom_icon_name);
    applemenu->custom_icon_name = g_strdup(xfce_rc_read_entry(rc, "custom-icon-name", APPLEMENU_ICON_NAME));
    
    g_free(applemenu->app_store_command);
    applemenu->app_store_command = g_strdup(xfce_rc_read_entry(rc, "app-store-command", DEFAULT_APP_STORE_COMMAND));
    
    applemenu->transparency = xfce_rc_read_int_entry(rc, "transparency", DEFAULT_TRANSPARENCY);
    
    /* Close config file */
    xfce_rc_close(rc);
    
    /* Update icon */
    gtk_image_set_from_icon_name(GTK_IMAGE(applemenu->icon), 
                                 applemenu->custom_icon_name, 
                                 GTK_ICON_SIZE_BUTTON);
    
    /* Apply transparency to both button and menu */
    if (applemenu->transparency < 100) {
        gtk_widget_set_opacity(GTK_WIDGET(applemenu->button), 
                              applemenu->transparency / 100.0);
        if (applemenu->menu) {
            gtk_widget_set_opacity(GTK_WIDGET(applemenu->menu), 
                                  applemenu->transparency / 100.0);
        }
    }
}

/* Configuration saving */
static void
applemenu_save_config(XfcePanelPlugin *plugin, AppleMenuPlugin *applemenu)
{
    gchar *file;
    XfceRc *rc;
    
    /* Get config file location */
    file = xfce_panel_plugin_save_location(plugin, TRUE);
    if (G_UNLIKELY(!file))
        return;
    
    /* Open config file */
    rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);
    
    if (G_UNLIKELY(!rc))
        return;
    
    /* Write settings */
    xfce_rc_write_bool_entry(rc, "show-recent-items", applemenu->show_recent_items);
    xfce_rc_write_int_entry(rc, "recent-items-max", applemenu->recent_items_max);
    xfce_rc_write_entry(rc, "custom-icon-name", applemenu->custom_icon_name);
    xfce_rc_write_entry(rc, "app-store-command", applemenu->app_store_command);
    xfce_rc_write_int_entry(rc, "transparency", applemenu->transparency);
    
    /* Close config file */
    xfce_rc_close(rc);
}

/* Configuration dialog response */
static void
applemenu_configure_response(GtkWidget *dialog, gint response, AppleMenuPlugin *applemenu)
{
    gboolean result;
    
    if (response == GTK_RESPONSE_HELP) {
        result = g_spawn_command_line_async("exo-open --launch WebBrowser " 
                                          "https://docs.xfce.org/xfce/xfce4-panel/start", 
                                          NULL);
        
        if (G_UNLIKELY(result == FALSE))
            g_warning(_("Unable to open the following url: %s"), 
                     "https://docs.xfce.org/xfce/xfce4-panel/start");
    } else {
        /* Save configuration on close */
        applemenu_save_config(applemenu->plugin, applemenu);
        
        /* Recreate menu with new settings */
        if (applemenu->menu) {
            gtk_widget_destroy(applemenu->menu);
            applemenu->menu = NULL;
        }
        applemenu_create_menu(applemenu);
        
        /* Hide dialog instead of destroying to avoid issues */
        gtk_widget_hide(dialog);
        
        /* Unblock panel menu */
        xfce_panel_plugin_unblock_menu(applemenu->plugin);
    }
}

/* Icon chooser callback */
static void
applemenu_icon_chooser_clicked(GtkWidget *button, AppleMenuPlugin *applemenu)
{
    GtkWidget *chooser;
    gchar *icon;
    
    chooser = exo_icon_chooser_dialog_new(_("Select An Icon"),
                                         GTK_WINDOW(gtk_widget_get_toplevel(button)),
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         NULL);
    
    gtk_dialog_set_default_response(GTK_DIALOG(chooser), GTK_RESPONSE_ACCEPT);
    exo_icon_chooser_dialog_set_icon(EXO_ICON_CHOOSER_DIALOG(chooser), 
                                     applemenu->custom_icon_name);
    
    if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_ACCEPT) {
        icon = exo_icon_chooser_dialog_get_icon(EXO_ICON_CHOOSER_DIALOG(chooser));
        
        g_free(applemenu->custom_icon_name);
        applemenu->custom_icon_name = icon;
        
        /* Update button icon */
        gtk_image_set_from_icon_name(GTK_IMAGE(applemenu->icon), 
                                     icon, GTK_ICON_SIZE_BUTTON);
        
        /* Update icon chooser button */
        gtk_image_set_from_icon_name(GTK_IMAGE(gtk_button_get_image(GTK_BUTTON(button))),
                                     icon, GTK_ICON_SIZE_DIALOG);
    }
    
    gtk_widget_destroy(chooser);
}

/* Transparency scale callback */
static void
applemenu_transparency_changed(GtkScale *scale, AppleMenuPlugin *applemenu)
{
    applemenu->transparency = (gint)gtk_range_get_value(GTK_RANGE(scale));
    
    /* Apply transparency to both button and menu */
    gtk_widget_set_opacity(GTK_WIDGET(applemenu->button), 
                          applemenu->transparency / 100.0);
    if (applemenu->menu) {
        gtk_widget_set_opacity(GTK_WIDGET(applemenu->menu), 
                              applemenu->transparency / 100.0);
    }
}

/* Configuration dialog */
static void
applemenu_configure_plugin(XfcePanelPlugin *plugin, AppleMenuPlugin *applemenu)
{
    GtkWidget *dialog, *content_area, *grid;
    GtkWidget *label, *entry, *button, *scale, *check;
    GtkWidget *icon;
    gint row = 0;
    
    /* Block plugin context menu */
    xfce_panel_plugin_block_menu(plugin);
    
    /* Create dialog */
    dialog = xfce_titled_dialog_new_with_buttons(_("Apple Menu Settings"),
                                                 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(plugin))),
                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 _("_Help"), GTK_RESPONSE_HELP,
                                                 _("_Close"), GTK_RESPONSE_OK,
                                                 NULL);
    
    /* Center dialog on the screen */
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name(GTK_WINDOW(dialog), "xfce4-settings");
    
    /* Connect response signal */
    g_signal_connect(G_OBJECT(dialog), "response",
                     G_CALLBACK(applemenu_configure_response), applemenu);
    
    /* Create grid for layout */
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 12);
    gtk_box_pack_start(GTK_BOX(content_area), grid, TRUE, TRUE, 0);
    
    /* Icon chooser */
    label = gtk_label_new_with_mnemonic(_("_Icon:"));
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    
    button = gtk_button_new();
    icon = gtk_image_new_from_icon_name(applemenu->custom_icon_name, GTK_ICON_SIZE_DIALOG);
    gtk_button_set_image(GTK_BUTTON(button), icon);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), button);
    g_signal_connect(G_OBJECT(button), "clicked",
                     G_CALLBACK(applemenu_icon_chooser_clicked), applemenu);
    gtk_grid_attach(GTK_GRID(grid), button, 1, row++, 1, 1);
    
    /* App Store command */
    label = gtk_label_new_with_mnemonic(_("_App Store command:"));
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    
    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), applemenu->app_store_command);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
    g_signal_connect(G_OBJECT(entry), "changed",
                     G_CALLBACK(g_free), applemenu->app_store_command);
    g_signal_connect(G_OBJECT(entry), "changed",
                     G_CALLBACK(g_strdup), &applemenu->app_store_command);
    g_object_set_data(G_OBJECT(entry), "applemenu", applemenu);
    g_signal_connect(G_OBJECT(entry), "focus-out-event",
                     G_CALLBACK(gtk_entry_get_text), entry);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, row++, 1, 1);
    
    /* Transparency */
    label = gtk_label_new_with_mnemonic(_("_Transparency:"));
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    
    scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 5);
    gtk_range_set_value(GTK_RANGE(scale), applemenu->transparency);
    gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), scale);
    g_signal_connect(G_OBJECT(scale), "value-changed",
                     G_CALLBACK(applemenu_transparency_changed), applemenu);
    gtk_grid_attach(GTK_GRID(grid), scale, 1, row++, 1, 1);
    
    /* Show recent items */
    check = gtk_check_button_new_with_mnemonic(_("Show _recent items"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), applemenu->show_recent_items);
    g_signal_connect(G_OBJECT(check), "toggled",
                     G_CALLBACK(gtk_toggle_button_get_active), &applemenu->show_recent_items);
    gtk_grid_attach(GTK_GRID(grid), check, 0, row++, 2, 1);
    
    /* Show dialog */
    gtk_widget_show_all(dialog);
}
