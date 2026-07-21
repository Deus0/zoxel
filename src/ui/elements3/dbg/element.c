entity dbg_element3;

void zox_dbg_spawn_element3(ecs *world, ClickEventData data) {
    byte dbg_inspector = 1;
    if (dbg_element3) {
        zox_log("+ Deleting [dbg_element3]");
        zox_delete(dbg_element3);
        dbg_element3 = 0;
        return;
    }
    entity camera = zox_getv(dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    zox_log("+ Testing [dbg_element3]");
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -1);
    entity e = spawn_canvas3(world, prefab_canvas3, spawn_position);
    spawn_element3(world, prefab_element3, e, float3_zero);
    dbg_element3 = e;
    if (dbg_inspector) {
        entity player = dbg_player;
        entity canvas = zox_getv(player, CanvasLink);
        spawn_inspector(world, canvas, player, e);
    }
}
