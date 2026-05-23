#include "towns.c"
#include "walls.c"

void zox_define_systems_towns(ecs* world) {
    zox_system(
        TownMapSystem,
        EcsPreStore,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] tunks.BiomeMap,
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
