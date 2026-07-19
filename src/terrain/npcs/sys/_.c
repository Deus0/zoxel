#include "spawn.c"
#include "despawn.c"
#include "can_spawn.c"

void define_systems_characters3_terrain(ecs* world) {
    zox_system(
        Characters3SpawnZoneSystem,
        zoxp_update,
        [in] chunks3.ChunkLodDirty,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [in] chunks.GenerateChunk,
        [out] terrains.npcs.NpcSpawnZone,
        [out] terrains.npcs.NpcSpawnZoneDirty,
        [none] terrains.TerrainChunk
    );
    zox_system(
        Characters3DespawnSystem,
        zoxp_update,
        [in] terrains.npcs.NpcSpawnZoneDirty,
        [in] terrains.npcs.NpcSpawnZone,
        [out] terrains.npcs.CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrains.TerrainChunk
    );
    // NOTE: Writes to VoxelNode
    zox_system_1(
        Characters3SpawnSystem,
        zoxp_mainthread,
        [in] terrains.npcs.NpcSpawnZoneDirty,
        [in] terrains.npcs.NpcSpawnZone,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDisabled,
        [in] chunks3.ChunkPosition,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [out] terrains.npcs.CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrains.TerrainChunk
    );
}
