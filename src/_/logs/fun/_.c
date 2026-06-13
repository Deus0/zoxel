// log implementation
#include "prefix.c"
#ifndef zox_disable_logs
    #ifdef zox_android
        #define zox_log_(msg, ...) \
            __android_log_print(ANDROID_LOG_INFO, "SDL", "zox_log: "msg, ##__VA_ARGS__);
    #else
        #include "default.c"
        #define ARG_COUNT(...) (ARG_COUNT_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
        # define ARG_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, count, ...) count
        #define zox_log_(msg, ...) {\
            if (!ARG_COUNT(__VA_ARGS__)) {\
                zox_log__(msg);\
            } else {\
                zox_log_android(msg, ##__VA_ARGS__);\
            }\
        }
    #endif

    int clear_logs() {
#ifdef log_to_file
        fclose(fopen("log.txt", "w"));
#endif
        return 0;
    }
#else
    #define zox_log_(msg, ...) { }
    int clear_logs() { return 0; }
#endif

#ifndef zox_disable_logs
    #define zox_log(...) zox_log_prefix(NULL, "", __VA_ARGS__);
    #define zox_logv(...) if (zox_verbose) zox_log_prefix(NULL, "[INFO] ", __VA_ARGS__)
    #define zox_logw(...) zox_log_prefix(zox_log_colors_yellow, "[WARNING] ", __VA_ARGS__);
    #define zox_loge(...) zox_log_prefix(zox_log_colors_red, "[ERROR] ", __VA_ARGS__);
    #define zox_log_error(...) zox_log_prefix(zox_log_colors_red, "[ERROR] ", __VA_ARGS__);
    #define zox_log_errno(format, ...) zox_log(format ": [%s]", __VA_ARGS__, strerror(errno))
#else
    #define zox_log(...) { }
    #define zox_logv(...) { }
    #define zox_logw(...) { }
    #define zox_loge(...) { }
    #define zox_log_error(...) { }
    #define zox_log_errno(format, ...) { }
#endif
