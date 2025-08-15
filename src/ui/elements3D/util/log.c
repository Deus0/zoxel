// #define zox_enable_log_elements3D
// #define zox_enable_log_text3D
// #define zox_enable_log_text

#if defined(zox_enable_log_elements3D) && !defined(zox_disable_logs)
    #define zox_log_elements3D(...) zox_log(##__VA_ARGS__)
#else
    #define zox_log_elements3D(...) { }
#endif

#if defined(zox_enable_log_text) && !defined(zox_disable_logs)
    #define zox_log_text(...) zox_log(##__VA_ARGS__)
#else
    #define zox_log_text(...) { }
#endif

#if defined(zox_enable_log_text3D) && !defined(zox_disable_logs)
    #define zox_log_text3D(...) zox_log(##__VA_ARGS__)
#else
    #define zox_log_text3D(...) { }
#endif