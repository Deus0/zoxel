entity2 spawn_button(ecs* world, entity prefab, entity parent, const char* text, int2 position, int2 size, float2 position_anchor, byte alignment, byte font_size, byte2 padding, color fill, color outline, color font_fill, color font_outline) {
    int text_length = text != NULL ? strlen(text) : 0;
    if (!size.x || !size.y) {
        size = (int2) {font_size * text_length, font_size };
        size.x += padding.x * 2;
        size.y += padding.y * 2;
    }
    zox_instance(prefab);
    zox_name("button");
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, FillColor, { fill });
    zox_set(e, ElementFillColor, { fill });
    zox_set(e, OutlineColor, { outline });
    zox_set(e, ElementOutlineColor, { outline });
    entity e2 = spawn_text(world, prefab_text, e, int2_zero, alignment_to_anchor(alignment), font_size, alignment, padding, text, font_fill, font_outline);
    return (entity2) { e, e2 };
}

entity spawn_button_old(ecs *world, LayoutParentData canvas_data, LayoutParentData parent, ElementSpawnData element_data, SpawnTextData zext_data, SpawnButtonData button_data) {
    int text_length = zext_data.text != NULL ? strlen(zext_data.text) : 0;
    int2 size = (int2) { zext_data.font_size * text_length, zext_data.font_size };
    element_data.size = (int2) {
        size.x + zext_data.margins.x * 2,
        size.y + zext_data.margins.y * 2
    };
    zox_instance(element_data.prefab);
    zox_name("button");
    zox_set(e, FillColor, { button_data.fill });
    zox_set(e, ElementFillColor, { button_data.fill });
    zox_set(e, OutlineColor, { button_data.outline });
    zox_set(e, ElementOutlineColor, { button_data.outline });
    set_element_spawn_data(world, e, canvas_data, parent, element_data);
    entity text = spawn_text(world, button_data.prefab_text, e, int2_zero, float2_half, zext_data.font_size, zext_data.alignment, zext_data.margins, zext_data.text, zext_data.font_fill_color, zext_data.font_outline_color);
    zox_set(text, Layer2D, { element_data.layer + 1 });
    return e;
}
