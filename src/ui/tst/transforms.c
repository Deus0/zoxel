 entity zox_dbg_element_transforms;

void zox_dbg_spawn_element_transforms(ecs *world, ClickEventData data) {
    entity player = dbg_player;
    zox_log("Testing [element]: %lu", zox_dbg_element_transforms);
    if (zox_dbg_element_transforms) {
        zox_delete(zox_dbg_element_transforms);
        zox_dbg_element_transforms = 0;
        return;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    byte layer = 32;
    zox_log("Test Spawning [zox_dbg_element_transforms] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    entity e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, canvas_size, float2_half, layer);
    // zox_set(canvas, WindowToTop, { zox_dbg_element_transforms });
    byte size = ui_scale * 32;
    {
        int2 position = (int2) { 0, size };
        entity e2 = spawn_uic(world, prefab_element_frame, e, float2_half, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
        zox_set(e2, Layer2D, { layer + 1 });
    }
    {
        int2 position = (int2) { 0, 0 };
        entity e2 = spawn_uic(world, prefab_element_frame, e, float2_half, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
        zox_set(e2, Layer2D, { layer + 1 });
        zox_set(e2, Scale1, { 0.5f });
    }
    {
        int2 position = (int2) { size * 2, 0 };
        entity e2 = spawn_uic(world, prefab_element_frame, e, float2_half, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
        zox_set(e2, Layer2D, { layer + 1 });
        zox_set(e2, Scale2, {{ -3, 0.5f }});
        zox_add(e2, TransformMatrix);
        {
            int2 position = (int2) { size, 0 };
            entity e3 = spawn_uic(world, prefab_element_frame, e2, float2_half, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
            zox_set(e3, Layer2D, { layer + 2 });
            zox_set(e3, Scale1, { 0.25f });
            zox_add(e3, TransformMatrix);
        }
    }
    zox_dbg_element_transforms = e;
}
