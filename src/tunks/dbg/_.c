#include "regions.c"
#include "heights.c"

void zox_define_systems_tunks_debug(ecs* world) {
    zox_system(
        RegionTextureSystem,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.RegionTexture
    );
    zox_system(
        HeightsTextureSystem,
        zoxp_update,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.HeightsTexture
    );
}
