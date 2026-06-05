#include "spawn.c"
#include "despawn.c"
#include "can_spawn.c"

void define_systems_characters3_terrain(ecs* world) {
    zox_system(
        Characters3SpawnZoneSystem,
        EcsPreUpdate,
        [in] core.Loaded,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [out] CharacterSpawnZone
    );
    zox_system(
        Characters3DespawnSystem,
        EcsPostUpdate,
        [in] CharacterSpawnZone,
        [out] CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrain.TerrainChunk
    );
    // NOTE: Writes to VoxelNode
    zox_system_1(
        Characters3SpawnSystem,
        zoxp_mainthread,
        [in] CharacterSpawnZone,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDisabled,
        [in] chunks3.ChunkPosition,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [out] CharactersSpawned,
        [out] chunks3.ChunkEntities,
        [none] terrain.TerrainChunk
    );
}
