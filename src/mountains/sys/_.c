#include "spawn.c"
#include "maps.c"

void zox_define_systems_mountains(ecs* world) {
    zox_system_1(
        RegionMountainSystem,
        zoxp_mainthread,
        [in] core.Generate,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [none] regions.Region
    );
    zox_system(
        MountainMapSystem,
        EcsPostUpdate,
        [in] core.Generate,
        [in] regions.RegionLink,
        [in] tunks.TunkPosition,
        [out] tunks.VegetationMap,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
}
