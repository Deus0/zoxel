byte fullscreen = 1;
byte vsync = 0; // 1;
byte is_log_monitors = 1;
byte monitor = 0;
byte maximized = 1;
int2 default_window_position = { 0, 0 };
const int2 default_window_size = { 1280, 720 };
byte minimized = 0;
byte disable_apps_decor = 0;
byte using_gpu = 1;
byte audio_enabled = 0;
byte nosounds = 0;
#ifdef zox_android
byte window_resizeable = 0;
#else
byte window_resizeable = 1;
#endif
byte rendering = 1;
byte is_split_screen = 0;
byte halfscreen = 0;
byte screen_index = 0;
byte screens_count = 1; // assume
