void zox_dbg_spawn_popup3(ecs *world, ClickEventData data) {
    zox_log("+ Testing [popup3]");
    entity camera = zox_getv(dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -1);
    spawn_popup3_easy(world, "hello world", color_red, spawn_position,  2.5f, randf_range(4, 8));
}
