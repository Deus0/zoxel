#include "region_maps.c"

void zox_define_systems_tunks_debug(ecs* world) {
    zox_system(
        TunkTextureSystem,
        zoxp_update,
        [in] core.Generate,
        [in] tunks.TunkLink,
        [out] textures.TextureData,
        [out] rendering.TextureSize,
        [out] rendering.TextureDirty,
        [none] tunks.RegionTexture
    );
}
