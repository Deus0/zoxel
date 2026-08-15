#include "spawn.c"
#include "despawn.c"
#include "can_spawn.c"
#include "settings.c"

void define_systems_characters3_terrain(ecs* world) {
    zox_system(
        Characters3SpawnZoneSystem,
        zoxp_update,
        // [in] chunks3.ChunkLodDirty,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [out] npcs.NpcSpawnZone,
        [out] npcs.NpcSpawnZoneDirty,
        [none] terrains.TerrainChunk
    );
    zox_system(
        TerrainCharactersDespawnSystem,
        zoxp_update,
        [in] npcs.NpcSpawnZoneDirty,
        [in] npcs.NpcSpawnZone,
        [out] npcs.ChunkCharacters,
        [out] npcs.CharactersSpawned,
        [none] terrains.TerrainChunk
    );
    // NOTE: Writes to VoxelNode
    zox_system_1(
        TerrainCharactersSpawnSystem,
        zoxp_mainthread,
        [in] npcs.NpcSpawnZoneDirty,
        [in] npcs.NpcSpawnZone,
        [in] core.Seed,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDisabled,
        [in] chunks3.ChunkPosition,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [out] npcs.ChunkCharacters,
        [out] npcs.CharactersSpawned,
        [none] terrains.TerrainChunk
    );
    zox_system_1(
        NpcsSettingsSystem,
        zoxp_mainthread,
        [in] settings.LoadSettings
    );
    zox_system_1(
        NpcsSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
