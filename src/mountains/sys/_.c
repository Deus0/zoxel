#include "maps.c"
#include "regions.c"

void zox_define_systems_mountains(ecs* world) {
    zox_system_1(
        RegionMountainSystem,
        zoxp_mainthread,
        [in] core.Generate,
        [in] regions.RegionPosition,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [none] regions.Region
    );
    zox_system(
        MountainMapSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] regions.RegionLink,
        [in] chunks2.Chunk2Position,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
}
