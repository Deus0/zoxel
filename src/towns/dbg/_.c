#include "maps.c"
#include "maps_dump.c"

void zox_systems_towns_debug(ecs* world) {
    zox_system(
        town_texture_system,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] towns.TownTexture
    );
    // --maps: off until asked for (maps_dump.c)
    zox_system_1(
        maps_dump_system,
        zoxp_dbg_end,
        0
    );
    zox_set_enabled_id(zox_id(maps_dump_system), 0);
    add_hook_terminal_command(process_arguments_maps_dump);
}
