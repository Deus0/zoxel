#include "character.c"
#include "resize.c"
#include "text.c"

#if defined(zox_enable_log_text) && !defined(zox_disable_logs)
    #define zox_log_text(...) zox_log(##__VA_ARGS__)
#else
    #define zox_log_text(...) { }
#endif