#include "spawn.c"
#include "maps.c"
#include "walls.c"
#include "homes.c"

void zox_define_systems_towns(ecs* world) {
    zox_system(
        TownMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] tunks.HeightMap,
        [out] vegetation.VegetationMap,
        [out] towns.TownMap,
        [none] tunks.Tunk
    );
    // NOTE: Before vegetation atm
    zox_system(
        TownWallsSystem,
        zoxp_update,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );
    zox_system_1(
        RegionTownsSystem,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [out] regions.GenerateRegion,
        [none] regions.Region
    );
    zox_system_1(
        HomesSpawnSystem,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] towns.TownSize,
        [out] towns.GenerateTown,
        [none] towns.Town
    );
}
