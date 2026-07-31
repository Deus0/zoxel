entity spawn_popup3(ecs *world, const char* text, byte font_size, color font_fill, color font_outline, float3 position, float scale, float lifetime) {
    size_t length = strlen(text);
    int2 canvas_size  = { length * font_size, font_size };
    entity e = spawn_canvas3(world, prefab_canvas3, position, scale, canvas_size);
    zox_set_unique_name(e, "popup3");
    entity e2 = spawn_text(world, prefab_text3, e, int2_zero, float2_centre, font_size, zox_alignment_centre, byte2_zero, text, font_fill, font_outline);
    zox_setv(e2, LayoutSize, canvas_size);
    if (lifetime) {
        zox_setv(e, DestroyInTime, lifetime);
    }
    return e;
}

entity spawn_popup3_easy(ecs *world, const char *text, color font_fill, float3 position, float scale, float lifetime) {
    color font_outline = (color) { 15, 15, 15, 255 };
    byte font_size = text3D_resolution;
    entity e = spawn_popup3(world, text, font_size, font_fill, font_outline, position, scale, lifetime);
    if (lifetime) {
        float3 end = float3_add(position, float3_scale(float3_up, randf_range(0.1f, 0.15f)));
        lerp_to_position(world, e, 0.02, randf_range(0.4f, 0.6f), position, end);
    }
    return e;
}