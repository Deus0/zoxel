#include "maps.c"

void zox_define_systems_towns_debug(ecs* world) {
    zox_system(
        TownTextureSystem,
        zoxp_update,
        [in] core.Generate,
        [in] tunks.TunkLink,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [out] rendering.TextureDirty,
        [none] towns.TownTexture
    );
}
