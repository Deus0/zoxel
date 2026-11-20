// windows dev builds: logs an error for a gl call off the main thread (GLEW_GET_FUN, includes.c), a test fails on it
//  - gl has one context, current on the main thread: a call from a flecs worker races the driver's own threads
//  - glew's loaded functions, and gl 1.1's draws, textures, clears and reads (opengl32's own, macros in includes.c)
//  - zox_gl_sync=1 (env): a glFinish before each gl call, so a crash in the driver's own thread (nvidia's threaded
//    optimization) happens while the main thread waits on the call after the bad one, and the crash handler logs
//    the last calls and where they were made (addr2line -f -e <exe> <offset>)
#if defined(zox_windows) && defined(zox_logs)
#define zox_gl_history 8
static DWORD zox_gl_main_thread = 0;
static volatile LONG zox_gl_off_thread_count = 0;
static byte zox_gl_sync = 0;
static const char* zox_gl_names[zox_gl_history];
static void* zox_gl_callers[zox_gl_history];
static void* zox_gl_callers2[zox_gl_history];
static uint zox_gl_calls = 0;

// the first gl call sets the main thread (opengl's init, on the main thread)
__attribute__((noinline)) void zox_gl_thread_check(const char* name) {
    const DWORD thread = GetCurrentThreadId();
    if (!zox_gl_main_thread) {
        zox_gl_main_thread = thread;
        const char* sync = getenv("zox_gl_sync");
        zox_gl_sync = sync && sync[0] == '1';
        if (zox_gl_sync) {
            zox_log("+ [zox_gl_sync] a glFinish before every gl call");
        }
    } else if (thread != zox_gl_main_thread) {
        if (InterlockedIncrement(&zox_gl_off_thread_count) <= 16) {
            zox_loge("gl off the main thread [%s] thread [%lu] main [%lu]", name, (unsigned long) thread,
                (unsigned long) zox_gl_main_thread);
        }
        return;
    }
    if (zox_gl_sync) {
        glFinish();
        zox_gl_names[zox_gl_calls % zox_gl_history] = name;
        zox_gl_callers[zox_gl_calls % zox_gl_history] = __builtin_return_address(0);
        zox_gl_callers2[zox_gl_calls % zox_gl_history] = __builtin_return_address(1);
        zox_gl_calls++;
    }
}

// the crash handler's: the last gl calls made, the newest last
void zox_gl_log_last() {
    if (!zox_gl_sync) {
        return;
    }
    const char* base = (const char*) GetModuleHandleA(NULL);
    const uint count = zox_gl_calls < zox_gl_history ? zox_gl_calls : zox_gl_history;
    for (uint i = zox_gl_calls - count; i < zox_gl_calls; i++) {
        zox_loge("  gl call [%u] [%s] at +[0x%llx] from +[0x%llx]", i, zox_gl_names[i % zox_gl_history],
            (unsigned long long) ((const char*) zox_gl_callers[i % zox_gl_history] - base),
            (unsigned long long) ((const char*) zox_gl_callers2[i % zox_gl_history] - base));
    }
}
#endif
