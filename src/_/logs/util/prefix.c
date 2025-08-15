static inline void zox_log_prefix(
    const char* prefix,
    const char* fmt,
    ...
) {
    va_list args;
    va_start(args, fmt);
    // prepend prefix to fmt
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s %s\n", prefix, fmt);
    vprintf(buf, args);  // or your zox_log_ implementation
    va_end(args);
}