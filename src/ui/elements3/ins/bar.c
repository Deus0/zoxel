// TODO: Just use 2D bar with 3D prefab
entity3 spawn_bar3(ecs *world, float3 position, float scale, int2 size, byte font_size, color fill, color outline,  color fill2, color outline2, color font_fill, color font_outline, entity owner, float trail_offset) {
    entity3 output = { 0 };
    byte2 text_padding = byte2_single(12);
    entity e = spawn_canvas3(world, prefab_canvas3, position, scale, size);
    zox_set_unique_name(e, "bar3");
    output.x = e;
    {
        entity panel = spawn_uic(
            world,
            prefab_frame3,
            e,
            float2_centre,
            int2_zero,
            size,
            size,
            fill,
            outline);
        output.y = panel;
        zox_add(panel, Elementbar);
        zox_setv(panel, Layer2D, 0);
        zox_setv(panel, BarLevel, 1);
        zox_set_unique_name(panel, "bar_panel");
        {
            float2 anchor = float2_centre;
            int2 front_size = (int2) { size.x / 2, size.y };
            int2 front_position = (int2) { -size.x / 4, 0 };
            entity e2 = spawn_uic(
                world,
                prefab_frame3,
                panel,
                anchor,
                front_position,
                front_size,
                front_size,
                fill2,
                outline2);
            zox_add(e2, BarFront);
            zox_setv(e2, Layer2D, 1);
            zox_set_unique_name(e2, "frontbar");
        }
        {
            entity e2 = spawn_text(
                world,
                prefab_text3,
                panel,
                int2_zero,
                float2_centre,
                font_size,
                zox_alignment_centre,
                text_padding,
                "Health",
                font_fill,
                font_outline);
            output.z = e2;
            zox_setv(e2, LayoutSize, size);
            zox_setv(e2, Layer2D, 2);
            zox_set_unique_name(e2, "bar_label");
        }
    }
    if (owner) {
        zox_set(e, UIHolderLink, { owner });
        zox_set(e, UITrail, { (float3) { 0, trail_offset, 0 } });
    }
    return output;
}
