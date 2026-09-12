entity dbg_element3_tree;

static void spawn_element3_tree(
    ecs *world,
    entity parent,
    int2 canvas_size,
    byte depth,
    byte max_depth,
    int2 size)
{
    if (depth >= max_depth) {
        return;
    }
    int x_offset = canvas_size.x >> (depth + 2);
    int y_offset = canvas_size.y / (max_depth + 1);
    int s = int_clamp(size.x - 6, 8, size.x);
    int2 child_size = int2_single(s);
    entity left = spawn_uic(
        world,
        prefab_frame3,
        parent,
        float2_centre,
        (int2){ -x_offset, -y_offset },
        child_size,
        child_size,
        color_green,
        color_black);
    zox_setv(left, Layer, 3);
    entity right = spawn_uic(
        world,
        prefab_frame3,
        parent,
        float2_centre,
        (int2){ x_offset, -y_offset },
        child_size,
        child_size,
        color_red,
        color_black);
    zox_setv(right, Layer, 3);
    char name[64];
    snprintf(name, sizeof(name), "tree_%u_l", depth);
    zox_set_unique_name(left, name);
    snprintf(name, sizeof(name), "tree_%u_r", depth);
    zox_set_unique_name(right, name);
    spawn_element3_tree(world, left, canvas_size, depth + 1, max_depth, child_size);
    spawn_element3_tree(world, right, canvas_size,  depth + 1, max_depth, child_size);
}

void zox_dbg_spawn_element3_tree(ecs *world, ClickEventData data) {
    int2 canvas_size = int2_single(512);
    int2 element_size = int2_single(64);
    canvas_size.x *= 2;
    float canvas_scale = 0.25f;
    float distance = 0.25f;
    if (dbg_element3_tree) {
        zox_log("+ Deleting [dbg_element3_tree]");
        zox_delete(dbg_element3_tree);
        dbg_element3_tree = 0;
        return;
    }
    entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    zox_log("+ Testing [dbg_element3]");
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -distance);
    entity e = spawn_canvas3(world, prefab_canvas3, spawn_position, canvas_scale, canvas_size);
    zox_set_unique_name(e, "dbg_canvas3");
    dbg_element3_tree = e;
    {
        entity e2 = spawn_uic(world, prefab_frame3, e, float2_centre, int2_zero, canvas_size, canvas_size, (color) { 0, 155, 155, 155 }, color_black);
        zox_setv(e2, Layer, 0);
        zox_set_unique_name(e2, "dbg_background");
    }
    // Hierarchy Stress Test
    {
        entity root = spawn_uic(
            world,
            prefab_frame3,
            e,
            float2_top,
            (int2) { 0, -element_size.y },
            element_size,
            element_size,
            color_blue,
            color_black);
        zox_setv(root, Layer, 3);
        zox_set_unique_name(root, "tree_root");
        spawn_element3_tree(
            world,
            root,
            int2_sub(canvas_size, element_size),
            0,
            7,          // 255 entities including the root
            element_size);
    }
}
