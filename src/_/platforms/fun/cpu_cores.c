byte get_cpu_count() {
#if defined(zox_web)
    int cores = SDL_GetNumLogicalCPUCores();
    return cores > 0 ? (byte) cores : 1;
#elif defined(zox_windows)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (sysinfo.dwNumberOfProcessors > 255) ? 255 : (byte) sysinfo.dwNumberOfProcessors;
#elif defined(_SC_NPROCESSORS_ONLN)
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    return (nprocs < 1 ? 1 : (byte) nprocs);
#else
    return 1;
#endif
}
