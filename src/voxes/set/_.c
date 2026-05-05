#define directory_voxes "voxes"
const byte max_vox_file_lods = 5;
int files_voxes_count = 0;
entity *files_voxes;
string_hashmap *files_hashmap_voxes;

// GenerateVox == 1
const byte is_generate_vox_airs = 1;
const float fracture_dark_multiplier = 0.6f; // 0.13f
const float grass_blend_dark_multiplier = 0.8f; // 0.13f
byte disable_block_voxes = 0;
byte disable_block_vox_generation = 0;
byte is_generate_vox_outlines = 1;

#include "outlines.c"

void process_arguments_voxes(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--disableoutlines") == 0) {
            is_generate_vox_outlines = 0;
        } else if (strcmp(args[i], "--enableoutlines") == 0) {
            zox_log("+ enabling block outlines")
            is_generate_vox_outlines = 1;
        } else if (strcmp(args[i], "--noblockvoxes") == 0 || strcmp(args[i], "--nominivoxes") == 0) {
            zox_log("+ disabling block voxes")
            disable_block_voxes = 1;
        }
    }
}

void initialize_settings_voxes(ecs *world) {
   // zox_log("initialize_settings_voxes %i", is_generate_vox_outlines);
    zoxs_new_byte("outlines", set_block_outlines, is_generate_vox_outlines);
}
