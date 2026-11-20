// dev builds: logs an error for a gl buffer bound after it was deleted, with where it was bound
//  (addr2line -f -e <exe> <offset>): a name freed while something still links it draws garbage, or crashes the
//  driver (nvidia read its missing data from null)
//  - main thread only, as gl is (gl_thread.c)
#ifdef zox_logs
static byte* zox_gl_buffer_states = NULL;   // by name: 0 never made, 1 made, 2 deleted
static uint zox_gl_buffer_states_length = 0;
static uint zox_gl_deleted_binds = 0;

static inline byte* zox_gl_buffer_state(uint name) {
    if (name >= zox_gl_buffer_states_length) {
        uint length = zox_gl_buffer_states_length ? zox_gl_buffer_states_length : 4096;
        while (length <= name) {
            length *= 2;
        }
        byte* grown = realloc(zox_gl_buffer_states, length);
        if (!grown) {
            return NULL;
        }
        memset(grown + zox_gl_buffer_states_length, 0, length - zox_gl_buffer_states_length);
        zox_gl_buffer_states = grown;
        zox_gl_buffer_states_length = length;
    }
    return &zox_gl_buffer_states[name];
}

void zox_gl_buffer_made(uint name) {
    byte* state = zox_gl_buffer_state(name);
    if (state) {
        *state = 1;
    }
}

void zox_gl_buffer_deleted(uint name) {
    byte* state = zox_gl_buffer_state(name);
    if (state) {
        *state = 2;
    }
}

__attribute__((noinline)) void zox_gl_buffer_bound(uint name) {
    if (!name) {
        return;
    }
    byte* state = zox_gl_buffer_state(name);
    if (state && *state == 2 && ++zox_gl_deleted_binds <= 16) {
        void* caller = __builtin_return_address(0);
#ifdef zox_windows
        caller = (void*) ((const char*) caller - (const char*) GetModuleHandleA(NULL));
#endif
        zox_loge("gl buffer [%u] bound after it was deleted, at +[%p]", name, caller);
    }
}
#endif
