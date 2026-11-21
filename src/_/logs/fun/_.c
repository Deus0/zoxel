// log implementation
#include "prefix.c"

int clear_logs() { return 0; }

// counts every logged error, tests use it to fail fast on error spam
unsigned int zox_log_errors_count = 0;
// live gpu object counts, for leak checks (tst heartbeat logs them)
int zox_gpu_buffers_live = 0;
int zox_gpu_textures_live = 0;
int zox_gpu_buffers_freed = 0;
int zox_gpu_shaders_live = 0;
int zox_gpu_programs_live = 0;
int zox_gpu_fbos_live = 0;
int zox_gpu_rbos_live = 0;

#ifdef zox_logs

    #define zox_log(...) \
        zox_log_prefix(NULL, "", __VA_ARGS__);

    #define zox_logw(...) \
        zox_log_prefix(zox_log_colors_yellow, "[WARNING] ", __VA_ARGS__);

    #define zox_loge(...) \
        { zox_log_errors_count++; zox_log_prefix(zox_log_colors_red, "[ERROR] ", __VA_ARGS__); }

    #define zox_log_errno(format, ...) \
        zox_log(format ": [%s]", __VA_ARGS__, strerror(errno))

    // details for debugging: only with --verbose (or a zox_verbose build), the default log stays short
    #define zox_logv(...) \
        { if (is_verbose) zox_log_prefix(NULL, "", __VA_ARGS__); }

#else
    // never run, its arguments still count as used (no unused variable warnings), the compiler drops it
    #define zox_log(...) { if (0) zox_log_prefix(NULL, "", __VA_ARGS__); }
    #define zox_logv(...) zox_log(__VA_ARGS__)
    #define zox_logw(...) zox_log(__VA_ARGS__)
    #define zox_loge(...) zox_log(__VA_ARGS__)
    #define zox_log_errno(format, ...) { }
#endif
