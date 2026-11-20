// tst: binary_size, the size of the running binary (the one the player downloads), logged in KB
//  - checks it's a real file, and under zox_tst_binary_mb (env, in MB) when set; 0 or unset only logs
//  - debug builds are bigger than release: compare like with like
//  - run: bash bsh/tst.sh binary_size
//  - the binary is found with zox_executable_path (_/io): /proc/self/exe on linux, GetModuleFileName on windows
byte zox_tst_binary_size_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_binary_size_check(ecs* world) {
    (void) world;
    char path[1024];
    struct stat info;
    const byte found = zox_executable_path(path, sizeof(path)) && stat(path, &info) == 0;
    zox_tst_check(found, "couldn't stat the running binary");
    if (!found) {
        return;
    }
    const long long bytes = (long long) info.st_size;
    zox_log("[tst] [metric] binary size is [%lld] KB", bytes / 1024);
    zox_tst_check(bytes > 0, "the binary is empty");
    const char* budget = getenv("zox_tst_binary_mb");
    const double mb = budget ? atof(budget) : 0;
    if (mb > 0) {
        zox_tst_check(bytes <= (long long) (mb * 1024 * 1024), "the binary is [%lld] KB, over the [%.1f] MB budget", bytes / 1024, mb);
    }
}
