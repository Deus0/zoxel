entity2 spawn_label3(ecs *world, const char* text, byte font_size, color fill, color outline, color font_fill, color font_outline, float scale, entity owner, float trail_offset) {
    entity2 output = { };
    byte2 padding = byte2_single(12);
    float3 spawn_position = float3_zero;
    // float canvas_scale = 0.05f;
    size_t length = strlen(text);
    int2 canvas_size  = {
        length * font_size + padding.x * 2,
        font_size + padding.y * 2
    };
    // zox_log("spawn_label3 canvas_size [%ix%i]", canvas_size.x, canvas_size.y);
    entity e = spawn_canvas3(world, prefab_canvas3, spawn_position, scale, canvas_size);
    zox_set_unique_name(e, "label3");
    output.x = e;
    {
        entity panel = spawn_uic(world, prefab_frame3, e, float2_centre, int2_zero, canvas_size, canvas_size, fill, outline);
        // zox_setv(panel, Layer, 0);
        {
            entity e2 = spawn_text(world, prefab_text3, panel, int2_zero, float2_centre, font_size, zox_alignment_centre, padding, text, font_fill, font_outline);
            zox_setv(e2, LayoutSize, canvas_size);
            zox_setv(e2, Layer, 1);
            output.y = e2;
        }
    }
    if (owner) {
        zox_set(e, UIHolderLink, { owner });
        zox_set(e, UITrail, { (float3) { 0, trail_offset, 0 } });
    }
    return output;
}