#include "maps.c"
#include "landfill.c"

void zox_systems_heights(ecs* world) {
    zox_system(
        HeightMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] tunks.HeightMap,
        [none] tunks.Tunk
    );
    zox_system(
        LandfillChunkSystem,
        zoxp_octree_write,
        [in] chunks3.ChunkPosition,
        [in] chunks.NodeDepth,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [none] terrains.TerrainChunk
    );
    add_system_process_counter(world, zox_id(LandfillChunkSystem));
}
