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
    //  - snprintf returns what it would write, so a long line put index past the buffer, and the next
    //      write overflowed it (a stack smash, long lines crashed), index is kept inside it
    int index = 0;
    char buffer[zox_log_lengths];
    const int end = (int) sizeof(buffer) - 2;   // room for the newline
    if (zox_logs_is_colors && color) {
        index+= snprintf(buffer + index, sizeof(buffer) - index, "%s", color);
        index = index > end ? end : index;
    }
    if (is_time_log_prefixes) {
        index += snprintf(buffer + index, sizeof(buffer) - index, "%s ", timestamp);
        index = index > end ? end : index;
    }
    index += snprintf(buffer + index, sizeof(buffer) - index, "%s", prefix);
    index = index > end ? end : index;
    va_list args;
    va_start(args, format);
    index += vsnprintf(buffer + index, sizeof(buffer) - index, format, args);
    va_end(args);
    index = index > end ? end : index;
    if (zox_logs_is_colors && color) {
        index += snprintf(buffer + index, sizeof(buffer) - index, "%s", zox_log_colors_reset);
        index = index > end ? end : index;
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
