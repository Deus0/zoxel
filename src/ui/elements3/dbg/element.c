entity dbg_element3;
entity dbg_element3_inspector;

void zox_dbg_spawn_element3(ecs *world, ClickEventData data) {
    int2 canvas_size = int2_single(64);
    float canvas_scale = 0.25f;
    // float3 canvas_scale = (float3) { 0.25f, 0.25f, 0 };
    if (dbg_element3) {
        zox_log("+ Deleting [dbg_element3]");
        zox_delete(dbg_element3);
        dbg_element3 = 0;
        if (zox_valid(dbg_element3_inspector)) {
            zox_delete(dbg_element3_inspector);
            dbg_element3_inspector = 0;
        }
        return;
    }
    entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    zox_log("+ Testing [dbg_element3]");
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -1);
    entity e = spawn_canvas3(world, prefab_canvas3, spawn_position, canvas_scale, canvas_size);
    zox_set_unique_name(e, "dbg_canvas3");
    entity e2 = spawn_element3(
        world,
        prefab_frame3,
        e,
        float2_centre,
        int2_zero,
        canvas_size);
    zox_set_unique_name(e2, "dbg_element3");
    dbg_element3 = e;
    inspect_entity(world, e);
}
