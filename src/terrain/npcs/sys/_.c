#include "spawn.c"
#include "despawn.c"
#include "can_spawn.c"

void define_systems_characters3_terrain(ecs* world) {
    zox_system(
        Characters3SpawnZoneSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] saves.Loaded,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [out] terrain.npcs.NpcSpawnZone,
        [out] terrain.npcs.NpcSpawnZoneDirty,
        [none] terrain.TerrainChunk
    );
    zox_system(
        Characters3DespawnSystem,
        EcsOnUpdate,
        [in] terrain.npcs.NpcSpawnZoneDirty,
        [in] terrain.npcs.NpcSpawnZone,
        [out] terrain.npcs.CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrain.TerrainChunk
    );
    // NOTE: Writes to VoxelNode
    zox_system_1(
        Characters3SpawnSystem,
        zoxp_mainthread,
        [in] terrain.npcs.NpcSpawnZoneDirty,
        [in] terrain.npcs.NpcSpawnZone,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDisabled,
        [in] chunks3.ChunkPosition,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [out] terrain.npcs.CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrain.TerrainChunk
    );
}
