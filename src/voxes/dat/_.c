byte disable_block_voxes = 0;
byte disable_block_vox_generation = 0;
#include "default_palette.c"
#include "vox_file.c"
#include "spawn_block_vox.c"
#include "settings.c"
#include "generation.c"
#include "outlines.c"

void initialize_settings_voxes(ecs *world) {
    zoxs_new_byte("outlines", set_block_outlines, is_generate_vox_outlines);
}