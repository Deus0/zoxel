#include "maps.c"
#include "walls.c"
#include "regions.c"

void zox_define_systems_towns(ecs* world) {
    zox_system_1(
        RegionTownsSystem,
        zoxp_mainthread,
        [in] core.Generate,
        [in] regions.RegionPosition,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [none] regions.Region
    );
    zox_system(
        TownMapSystem,
        EcsPreStore,
        [in] core.Generate,
        [in] regions.RegionLink,
        [in] chunks2.Chunk2Position,
        [in] tunks.BiomeMap,
        [out] tunks.VegetationMap,
        [out] towns.TownMap,
        [none] tunks.Tunk
    );
    zox_system(
        TownWallsSystem,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] voxes.VoxLink,
        [in] tunks.TunkLink,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [none] terrain.TerrainChunk
    );
}
