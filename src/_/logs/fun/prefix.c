static inline void zox_log_prefix(
    const char* color,
    const char* prefix,
    const char* format,
    ...)
{
    // build timestamp
    char timestamp[32];
    if (is_time_log_prefixes) {
        time_t now = time(NULL);
        struct tm t;
        localtime_r(&now, &t);
        strftime(timestamp, sizeof(timestamp), "[%H:%M:%S]", &t);
    }
    // prepend timestamp + prefix
    int index = 0;
    char buffer[zox_log_lengths];
    if (zox_logs_is_colors && color) {
        index+= snprintf(buffer + index, sizeof(buffer) - index, "%s", color);
    }
    if (is_time_log_prefixes) {
        index += snprintf(buffer + index, sizeof(buffer) - index, "%s ", timestamp);
    }
    index += snprintf(buffer + index, sizeof(buffer) - index, "%s", prefix);
    va_list args;
    va_start(args, format);
    index += vsnprintf(buffer + index, sizeof(buffer) - index, format, args);
    va_end(args);
    if (zox_logs_is_colors && color) {
        index += snprintf(buffer + index, sizeof(buffer) - index, "%s", zox_log_colors_reset);
    }
#ifdef zox_android
    __android_log_print(ANDROID_LOG_INFO, "SDL", "%s", buffer);
#else
    snprintf(buffer + index, sizeof(buffer) - index, "\n");
    fputs(buffer, stdout);
#endif
}
