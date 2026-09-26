static inline void zox_log_prefix(
    const char* color,
    const char* prefix,
    const char* format,
    ...)
{
    // build timestamp
    /*char timestamp[32];
    if (is_time_log_prefixes) {
        time_t now = time(NULL);
        struct tm t;
        localtime_r(&now, &t);
        strftime(timestamp, sizeof(timestamp), "[%H:%M:%S]", &t);
    }*/
    char timestamp[32];
    if (is_time_log_prefixes) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        struct tm t;
        localtime_r(&ts.tv_sec, &t);
        snprintf(timestamp, sizeof(timestamp),
                 "[%02d:%02d:%02d.%03ld]",
                 t.tm_hour, t.tm_min, t.tm_sec,
                 ts.tv_nsec / 1000000);
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
#if defined(zox_android)
    __android_log_print(ANDROID_LOG_INFO, "SDL", "%s", buffer);
#elif defined(zox_web)
    printf("%s\n", buffer);
    fflush(stdout);
#else
    snprintf(buffer + index, sizeof(buffer) - index, "\n");
    fputs(buffer, stdout);
#endif
}
