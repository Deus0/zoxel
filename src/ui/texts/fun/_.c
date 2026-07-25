#include "character.c"
#include "text.c"

// calculates the child index, takes out ascii like new line that have no zigel spawns
// NOTE: Skips new line characters
uint child_index_to_text_array_index(const byte *data, uint length, int child_index) {
    uint index = 0;
    for (uint i = 0; i < length; i++) {
        byte zigel = data[i];
        if (is_skip_zigel(zigel)) {
            continue;
        }
        if (index == child_index) {
            return i;
        }
        index++;
    }
    zox_logw("child_index_to_text_array_index: index [%i] child_index [%i] length [%i]", index, child_index, length);
    return length;
}