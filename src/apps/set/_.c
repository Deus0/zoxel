// app
int2 default_window_position = { 0, 0 };
const int2 default_window_size = { 1280, 720 };
byte minimized = 0;
byte disable_apps_decor = 0;

#ifdef zox_android
    byte window_resizeable = 0;
#else
    byte window_resizeable = 1;
#endif
byte vsync = 1;
byte rendering = 1;
byte is_split_screen = 0;
byte halfscreen = 0;
byte screen_index = 0;
byte screens_count = 1; // assume
#include "fullscreen.c"
#include "maximized.c"
#include "monitor.c"

// make our setting
void initialize_settings_apps(ecs* world) {
    zoxs_new_byte("fullscreen", set_app_fullscreen, fullscreen)
    zoxs_new_byte("maximized", set_app_maximized, maximized)
    zoxs_new_byte("monitor", set_app_monitor, monitor)
    if (is_on_phosh()) {
        zox_log("Phosh Detected. Disabling Decor.");
        disable_apps_decor = 1;
    } else {
        zox_log("Not on Phosh.");
    }
}
