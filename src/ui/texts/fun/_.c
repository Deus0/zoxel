#include "character.c"
#include "resize.c"
#include "text.c"

#if defined(zox_enable_log_text) && !defined(zox_disable_logs)
    #define zox_log_text(...) zox_log(##__VA_ARGS__)
#else
    #define zox_log_text(...) { }
#endif

// calculates the child index, takes out ascii like new line that have no zigel spawns
uint calculate_zigel_data_index(const byte *data, uint length, int spawn_index) {
    uint j = 0;
    for (uint i = 0; i < length; i++) {
        if (data[i] != zox_char_newline) {
            if (j == spawn_index) {
                return i;
            }
            j++;
        }
    }
    zox_loge("calculate_zigel_data_index: j [%i] spawn_index [%i] length [%i]", j, spawn_index, length);
    return 0;
}