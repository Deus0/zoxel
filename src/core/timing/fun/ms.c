// Function to get current time in milliseconds
#ifdef zox_windows

double current_time_in_seconds() {
    static LARGE_INTEGER freq;
    static int init = 0;
    if (!init) {
        QueryPerformanceFrequency(&freq);
        init = 1;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart / (double)freq.QuadPart;
}

double get_time_ms() {
    return current_time_in_seconds() / 1000.0;
}

#else

double current_time_in_seconds() {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    return current_time.tv_sec + (double) current_time.tv_nsec / 1000000000.0;
}

double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1000000.0);
}

#endif
