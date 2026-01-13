#include "spawn.c"
#include "despawn.c"
#include "can_spawn.c"

void define_systems_characters3_terrain(ecs* world) {
    zox_system(
        Characters3SpawnZoneSystem,
        EcsPreUpdate,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [out] CharacterSpawnZone
    );
    zox_system(
        Characters3DespawnSystem,
        EcsPostUpdate,
        [in] rendering.RenderDistanceDirty,
        [in] CharacterSpawnZone,
        [out] CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrain.TerrainChunk
    );
    // NOTE: Writes to VoxelNode
    zox_system_1(
        Characters3SpawnSystem,
        zoxp_voxels_read,
        [in] rendering.RenderDistanceDirty,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] chunks3.VoxelNodeLoaded,
        [in] chunks3.ChunkPosition,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDisabled,
        [in] voxes.VoxLink,
        [in] chunks3.ChunkNeighbors,
        [out] CharactersSpawned,
        [out] CharactersEverSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrain.TerrainChunk
    );
}
