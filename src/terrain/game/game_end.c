// End Game
void game_end_terrain(ecs_world_t *world, const ecs_entity_t game) {
    if (zox_game_type == zox_game_mode_3D) {
        zox_geter(game, RealmLink, realmLink)
        zox_geter(realmLink->value, TerrainLink, terrainLink)
        if (zox_valid(terrainLink->value)) {
            zox_delete(terrainLink->value)
            zox_set(realmLink->value, TerrainLink, { 0 })
        }
        local_terrain = 0;
    }
}
