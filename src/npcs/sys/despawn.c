// Npcs despawn where they are, not where they spawned (doc: homes, spawn.c)
//  - npc_despawn_system: an npc whose chunk is past the spawn distance is removed, wherever its home is
//  - terrain_characters_despawn_system: a home chunk leaving the zone only readies itself to spawn again, its npcs
//    still out walking near the player stay, when it's back it spawns the ones gone (by NpcHome)
void terrain_characters_despawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NpcSpawnZoneDirty);
    zox_sys_in(NpcSpawnZone);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NpcSpawnZoneDirty, dirty);
        zox_sys_i(NpcSpawnZone, active);
        if (dirty->value != zox_dirty_active || active->value) {
            continue;
        }
        zox_remove(e, CharactersSpawned);
    }
    zox_sys_on_end();
} zoxd_system(terrain_characters_despawn_system);

void npc_despawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity chunk = zox_get_link(world, e, ChunkLink);
        if (!zox_valid(chunk)) {
            continue;   // not placed in a chunk yet
        }
        // past the spawn distance and its margin (can_spawn.c), by distance: a near chunk still settling its
        //  lods has no spawn zone yet, an npc walking into it stays
        if (!zox_has(chunk, RenderDistance) || zox_getv(chunk, RenderDistance) <= character_spawn_distance + 1) {
            continue;
        }
        zox_delete(e);
    }
    zox_sys_on_end();
} zoxd_system(npc_despawn_system);
