entity dbg_tst_streamer;

void zox_dbg_spawn_streamer(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    if (zox_valid(dbg_tst_streamer)) {
        zox_delete(dbg_tst_streamer);
        dbg_tst_streamer = 0;
        return;
    }
    entity camera = zox_getv(player, CameraLink);
    float3 position = zox_getv(camera, Position3D);
    entity terrain = zox_getv(camera, StreamLink);
    zox_log("Testing [dbg_tst_streamer] at [%fx%fx%f]", position.x, position.y, position.z);
    entity e = spawn_streamer(world, prefab_streamer, terrain, position);
    zox_set_unique_name(e, "dbg_tst_streamer");
    zox_set(e, StreamerLevel, { 1 });
    dbg_tst_streamer = e;
}
