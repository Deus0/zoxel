#include "spawn.c"
#include "maps.c"

void zox_define_systems_mountains(ecs* world) {
    zox_system_1(
        RegionMountainSystem,
        zoxp_mainthread,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [out] regions.GenerateRegion,
        [none] regions.Region
    );
    zox_system(
        MountainMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] regions.RegionLink,
        [in] tunks.TunkPosition,
        [out] tunks.GenerateTunk,
        [out] tunks.VegetationMap,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
}
