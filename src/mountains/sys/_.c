#include "spawn.c"
#include "maps.c"

void zox_systems_mountains(ecs* world) {
    zox_system_1(
        region_mountain_system,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [out] regions.GenerateRegion,
        [none] regions.Region
    );
    zox_system(
        mountain_map_system,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [out] tunks.GenerateTunk,
        [out] vegetation.VegetationMap,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
}
