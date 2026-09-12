entity dbg_tst_streamer;

void zox_dbg_spawn_streamer(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    if (zox_valid(dbg_tst_streamer)) {
        zox_delete(dbg_tst_streamer);
        dbg_tst_streamer = 0;
        return;
    }
    entity camera = zox_get_link(world, player, CameraLink);
    entity terrain = local_terrain; // zox_getv(camera, StreamLink);
    if (!zox_valid(terrain)) {
        zox_loge("Invalid terrain for [dbg_tst_streamer]");
        return;
    }
    float3 position = zox_getv(camera, Position3D);
    zox_log("Testing [dbg_tst_streamer] at [%fx%fx%f]",
        position.x,
        position.y,
        position.z);
    entity e = spawn_streamer(
        world,
        prefab_streamer,
        terrain,
        position);
    zox_set_unique_name(e, "dbg_tst_streamer");
    zox_setv(e, StreamerLevel, 1);
    dbg_tst_streamer = e;
}
