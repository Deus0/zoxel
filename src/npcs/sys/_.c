#include "spawn.c"
#include "despawn.c"
#include "can_spawn.c"
#include "settings.c"

void zox_systems_characters3_terrain(ecs* world) {
    zox_system(
        characters_can_spawn_system,
        zoxp_update,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [out] npcs.NpcSpawnZone,
        [out] npcs.NpcSpawnZoneDirty,
        [none] terrains.TerrainChunk,
    );
    zox_system(
        terrain_characters_despawn_system,
        zoxp_update,
        [in] npcs.NpcSpawnZoneDirty,
        [in] npcs.NpcSpawnZone,
        [none] terrains.TerrainChunk,
        [none] npcs.CharactersSpawned,
    );
    // NOTE: Writes to VoxelNode
    zox_system_1(
        terrain_characters_spawn_system,
        zoxp_spawn,
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
        [none] terrains.TerrainChunk,
        [none] !npcs.CharactersSpawned,
    );
    zox_system_1(
        npcs_settings_system,
        zoxp_spawn,
        [in] settings.LoadSettings
    );
    zox_system(
        npcs_settings_dirty_system,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
