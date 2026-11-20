#ifdef zox_web
void update_ecs_web() {
    update_ecs(local_world);
}
#endif


#include "sampler.c"

void engine_end() {
    running = 0;
#ifdef zox_web
    emscripten_cancel_main_loop();
#endif
}

#ifdef __GLIBC__
    #include <execinfo.h>
#endif

// logs a backtrace (glibc only), resolve it with: addr2line -f -e <binary> <+offsets>
void handle_segfault(int sig) {
    zox_loge("Segfault Detected [%i]", sig);
#ifdef __GLIBC__
    void* frames[64];
    const int count = backtrace(frames, 64);
    fflush(stdout);
    backtrace_symbols_fd(frames, count, 2);
#endif
    zox_loge("Exiting...");
    fflush(stdout);
    _exit(128 + sig);
}

#ifdef zox_windows
// windows: a crash on any thread (signal(SIGSEGV) is the c runtime's, a crash in a driver's own thread skipped it):
//  logs the exception, the module it was in and the offset there (addr2line -f -e <module> <offset> for ours)
//  - nvoglv64 (nvidia's opengl) raises and handles access violations of its own, gdb stops on those: this only runs
//    for ones nothing handled, the crash itself
LONG WINAPI handle_unhandled_exception(EXCEPTION_POINTERS* info) {
    const DWORD code = info->ExceptionRecord->ExceptionCode;
    void* address = info->ExceptionRecord->ExceptionAddress;
    char module[MAX_PATH] = "?";
    HMODULE handle = NULL;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR) address, &handle)) {
        GetModuleFileNameA(handle, module, sizeof(module));
    }
    zox_loge("Crash Detected [0x%08lx] at [%p] in [%s] +[0x%llx] (thread [%lu])", (unsigned long) code, address, module,
        (unsigned long long) ((char*) address - (char*) handle), (unsigned long) GetCurrentThreadId());
    fflush(stdout);
    fflush(stderr);
    _exit(128 + SIGSEGV);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void handle_terminal_close(int sig) {
    zox_log("...[%i]", sig)
    zox_log("# SOOooo you wanna deffffyyy my orders? heh..")
    zox_log("*ominous noises*")
    engine_end();
}

// handles loop with special check on web builds
void main_loop(ecs *world) {
#ifdef zox_web
    emscripten_set_main_loop(&update_ecs_web, -1, 1); // old - 60, 1);
#else
    signal(SIGINT, handle_terminal_close);     // Handles closing from control + c
    signal(SIGSEGV, handle_segfault);
#ifdef zox_windows
    SetUnhandledExceptionFilter(handle_unhandled_exception);
#endif
    zox_sample_start();
    while (running) {
        update_ecs(world);
    }
    zox_sample_write();
#endif
}

void dispose_zox(ecs *world) {
    // dispose_update_loop();
    // dispose_post_update_loop();
    ecs_fini(world);
}
