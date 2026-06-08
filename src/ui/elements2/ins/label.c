entity spawn_label_background(ecs *world, entity prefab, entity parent, entity canvas, int2 position, float2 anchor, byte2 padding, const char* text, byte font_size, byte alignment, byte layer, int2 parent_position, int2 parent_size, color fill, color fillo, color font_fill, color font_fillo, byte render_disabled) {
    if (!zox_valid(canvas)) {
        zox_logw("Canvas invalid in [spawn_label_background]");
        return 0;
    }
    entity e = spawn_text(world, prefab, parent, position, anchor, font_size, alignment, padding, text, font_fill, font_fillo);
    zox_name("label_background");
    zox_set(e, FillColor, { fill });
    zox_set(e, OutlineColor, { fillo });
    zox_set(e, RenderDisabled, { render_disabled });
    zox_set(e, Layer2D, { layer });
    return e;
}
