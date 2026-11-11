// log implementation
#include "prefix.c"

int clear_logs() { return 0; }

// counts every logged error, tests use it to fail fast on error spam
unsigned int zox_log_errors_count = 0;
// live gpu object counts, for leak checks (tst heartbeat logs them)
int zox_gpu_buffers_live = 0;
int zox_gpu_textures_live = 0;
int zox_gpu_buffers_freed = 0;

#ifdef zox_logs

    #define zox_log(...) \
        zox_log_prefix(NULL, "", __VA_ARGS__);

    #define zox_logw(...) \
        zox_log_prefix(zox_log_colors_yellow, "[WARNING] ", __VA_ARGS__);

    #define zox_loge(...) \
        { zox_log_errors_count++; zox_log_prefix(zox_log_colors_red, "[ERROR] ", __VA_ARGS__); }

    #define zox_log_errno(format, ...) \
        zox_log(format ": [%s]", __VA_ARGS__, strerror(errno))

    #ifdef zox_verbose
        #define zox_logv(...) \
            if (is_verbose) \
                zox_log_prefix(NULL, "[INFO] ", __VA_ARGS__)
    #else
        #define zox_logv(...) { }
    #endif

#else
    #define zox_log(...) { }
    #define zox_logv(...) { }
    #define zox_logw(...) { }
    #define zox_loge(...) { }
    #define zox_log_errno(format, ...) { }
#endif
