// TODO: Force a refresh for sunlight and see if it helps

void zox_dbg_lights3_refresh_sunlight(ecs* world, ClickEventData data) {
    if (!zox_valid(local_terrain)) {
        return;
    }
    entity e = local_terrain;
    uint count = zox_get_children_count_by_id(world, e, zox_id(SunnyChunk));
    // zox_geter(e, ChunkLinks, chunks);
    zox_log("Refreshing Terrain Chunks Sunlights [%i]", count);
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (zox_valid(e2) && zox_has(e2, SunnyChunk)) {
                zox_set(e2, GenerateLights, { zox_generate_lights_start });
            }
        }
    }
}
