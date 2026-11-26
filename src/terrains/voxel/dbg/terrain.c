entity zox_dbg_terrain;

entity zox_dbg_spawn_terrain(ecs* world) {
    if (zox_dbg_terrain) {
        zox_delete(zox_dbg_terrain);
        zox_dbg_terrain = 0;
        return 0;
    }
    lint seed = 666;
    zox_dbg_terrain = spawn_terrain(
        world,
        local_realm,
        seed);
    return zox_dbg_terrain;
    // zox_setv(zox_dbg_terrain, GenerateRealm, zox_generate_realm_start);
}