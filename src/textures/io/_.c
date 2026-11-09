#define directory_textures "textures"
int files_count_textures = 0;
entity *files_textures;
string_hashmap *files_hashmap_textures;

zox_tag(SaveTexture);

#include "fun/_.c"
#include "pre/texture.c"
#include "sys/save.c"
#include "fun/assets.c"

void import_textures_io(ecs* world) {
    zox_module(textures_io);
    zox_module_dispose(dispose_textures_files);
    zoxd_tag(SaveTexture);
    add_hook_files_load(load_files_textures);
}
