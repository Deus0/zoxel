extern entity spawn_inspector(ecs*, entity, entity, entity);
entity dbg_popup3;

void zox_dbg_spawn_popup3(ecs *world, ClickEventData data) {
    byte dbg_inspector = 0;
    if (zox_valid(dbg_popup3)) {
        // delete?
    }
    zox_log("+ Testing [popup3]");
    entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    float distance = 1;
    float lifetime = randf_range(0.6f, 2.2f);
    float scale = randf_range(0.04f, 0.08f);
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);
    entity e = spawn_popup3_easy(
        world,
        "hello world",
        color_red,
        spawn_position,
        scale,
        lifetime);
    dbg_popup3 = e;
    if (dbg_inspector) {
        entity player = dbg_player;
        entity canvas = zox_get_link(world, player, Canvas);
        spawn_inspector(
            world,
            canvas,
            player,
            e);
    }
}
