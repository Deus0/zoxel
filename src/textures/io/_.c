#ifndef zoxm_textures_io
#define zoxm_textures_io

#define directory_textures "textures"
int files_count_textures = 0;
entity *files_textures;
string_hashmap *files_hashmap_textures;
zox_tag(SaveTexture);
#include "pre/file_texture.c"
#include "sys/textures_save_system.c"
#include "fun/texture_files.c"

void dispose_textures_io(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_files_textures();
}

zox_begin_module(TexturesIO)
    zox_module_dispose(dispose_textures_io)
    zoxd_tag(SaveTexture);
    add_hook_files_load(load_files_textures);
zox_end_module(TexturesIO)

#endif