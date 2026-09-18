#include "maps.c"

void zox_systems_towns_debug(ecs* world) {
    zox_system(
        TownTextureSystem,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] towns.TownTexture
    );
}
