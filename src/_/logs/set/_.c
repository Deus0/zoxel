// settings
#define zox_log_lengths 512
#define APPNAME "Zoxel"
#ifdef zox_verbose
static unsigned char is_verbose = 1;
#else
static unsigned char is_verbose = 0;
#endif
static unsigned char is_time_log_prefixes = 1;
// NOTE: Windows colors dont work
#if !defined(zox_windows) && !defined(zox_web)
    static unsigned char zox_logs_is_colors = 1;
#else
    static unsigned char zox_logs_is_colors = 0;
#endif
#define zox_log_colors_reset    "\x1b[0m"
#define zox_log_colors_red      "\x1b[31m"
#define zox_log_colors_green    "\x1b[32m"
#define zox_log_colors_yellow   "\x1b[33m"
#define zox_log_colors_blue     "\x1b[34m"
#define zox_log_colors_cyan     "\x1b[36m"
