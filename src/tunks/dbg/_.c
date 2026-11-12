#include "regions.c"
#include "heights.c"

void zox_systems_tunks_debug(ecs* world) {
    zox_system(
        region_texture_system,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.RegionTexture
    );
    zox_system(
        heights_texture_system,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.HeightsTexture
    );
}
