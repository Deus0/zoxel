entity dbg_element3_corners;

void zox_dbg_spawn_element3_corners(ecs *world, ClickEventData data) {
    int2 canvas_size = int2_single(256);
    int2 element_size = int2_single(64);
    float canvas_scale = 0.5f;
    if (dbg_element3_corners) {
        zox_log("+ Deleting [dbg_element3_corners]");
        zox_delete(dbg_element3_corners);
        dbg_element3_corners = 0;
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
    entity e = spawn_canvas3(world, prefab_canvas3, spawn_position, canvas_scale, canvas_size);
    zox_set_unique_name(e, "dbg_canvas3");
    {
        entity e2 = spawn_uic(world, prefab_element3, e, float2_centre, int2_zero, canvas_size, canvas_size, color_cyan, color_black);
        zox_setv(e2, Layer2D, 0);
        // spawn_element3(world, prefab_element3, e, float2_centre, int2_zero, canvas_size);
    }
    {
        entity e2 = spawn_uic(world, prefab_element3, e, float2_centre, int2_zero, element_size, element_size, color_green, color_black);
        zox_setv(e2, Layer2D, 2);
        /*entity e2 = spawn_element3(world, prefab_element3, e, float2_centre, int2_zero, element_size);
        zox_setv(e2, Layer2D, 1);*/
    }
    {
        int2 position = int2_zero;
        // int2 position = (int2) { -element_size.x / 2, -element_size.y / 2 };
        entity e2 = spawn_element3(world, prefab_element3, e, float2_top_right, position, element_size);
        zox_setv(e2, Layer2D, 1);
        zox_setv(e2, MeshAlignment, zox_alignment_top_right);
    }
    {
        int2 position = int2_zero; // (int2) { element_size.x / 2, -element_size.y / 2 };
        entity e2 = spawn_element3(world, prefab_element3, e, float2_top_left, position, element_size);
        zox_setv(e2, Layer2D, 1);
        zox_setv(e2, MeshAlignment, zox_alignment_top_left);
    }
    {
        int2 position = int2_zero; // (int2) { -element_size.x / 2, element_size.y / 2 };
        entity e2 = spawn_element3(world, prefab_element3, e, float2_bottom_right, position, element_size);
        zox_setv(e2, Layer2D, 1);
        zox_setv(e2, MeshAlignment, zox_alignment_bottom_right);
    }
    {
        int2 position = int2_zero; // (int2) { element_size.x / 2, element_size.y / 2 };
        entity e2 = spawn_element3(world, prefab_element3, e, float2_bottom_left, position, element_size);
        zox_setv(e2, Layer2D, 1);
        zox_setv(e2, MeshAlignment, zox_alignment_bottom_left);
    }
    dbg_element3_corners = e;
}
