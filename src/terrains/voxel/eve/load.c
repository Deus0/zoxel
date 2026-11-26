// NOTE: When terrain load ends, we move Game State forward
void terrain_state_load_end(ecs* world, entity terrain) {
    zox_log("Load Finished on Terrain [%s]",
        zox_getn(terrain));
    entity game = zox_get_parent_by_id(
        world,
        terrain,
        zox_id(Game));
    zox_log(" - Terrains Game [%s]",
        zox_getn(game));
}