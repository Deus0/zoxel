 entity zox_dbg_element_transforms;

void zox_dbg_spawn_element_transforms(ecs *world, ClickEventData data) {
    entity player = dbg_player;
    zox_log("Testing [element]: %lu", zox_dbg_element_transforms);
    if (zox_dbg_element_transforms) {
        zox_delete(zox_dbg_element_transforms);
        zox_dbg_element_transforms = 0;
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    byte layer = 32;
    zox_log("Test Spawning [zox_dbg_element_transforms] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    entity e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, canvas_size, float2_half, layer);
    byte size = ui_scale * 32;
    {
        int2 position = (int2) { 0, size * 2 };
        entity e2 = spawn_uic(
            world,
            prefab_element_frame,
            e,
            float2_half,
            position,
            int2_single(size),
            int2_single(size),
            default_fill_color_icon,
            default_outline_color_icon);
        zox_setv(e2, FrameCorner, 0);
        zox_add(e2, Scale2);
        zox_add(e2, TransformMatrix);
        zox_set_unique_name(e2, "long_ui");
        zox_set(e2, Layer, { layer + 1 });
        zox_set(e2, LocalScale2, {{ -5, 1 }});
        {
            int2 position = (int2) { size / 2, 0 };
            entity e3 = spawn_uic(
                world,
                prefab_element_frame,
                e2,
                float2_half,
                position,
                int2_single(size),
                int2_single(size),
                default_fill_color_icon,
                default_outline_color_icon);
            zox_setv(e3, FrameCorner, 0);
            zox_add(e3, Scale2);
            zox_add(e3, TransformMatrix);
            zox_set_unique_name(e3, "long_ui_marker");
            zox_set(e3, Layer, { layer + 2 });
            zox_set(e3, LocalScale2, {{ -0.1f, 2.0f }});
        }
    }
    zox_dbg_element_transforms = e;
}
