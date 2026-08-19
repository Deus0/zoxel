#include "regions.c"
#include "heights.c"

void zox_define_systems_tunks_debug(ecs* world) {
    zox_system(
        RegionTextureSystem,
        zoxp_update,
        [in] tunks.TunkLink,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.RegionTexture
    );
    zox_system(
        HeightsTextureSystem,
        zoxp_update,
        [in] tunks.TunkLink,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [none] tunks.HeightsTexture
    );
}
