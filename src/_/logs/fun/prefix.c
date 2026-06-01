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

static inline void zox_log_time_prefix(
    const char* prefix,
    const char* fmt,
    ...
) {
    // build timestamp
    time_t now = time(NULL);
    struct tm t;
    localtime_r(&now, &t);
    char ts[32];
    strftime(ts, sizeof(ts), "[%H:%M:%S]", &t);
    // prepend timestamp + prefix
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s %s %s\n", ts, prefix, fmt);
    // forward args
    va_list args;
    va_start(args, fmt);
    vprintf(buf, args); // swap with zox_log_ if needed
    va_end(args);
}
