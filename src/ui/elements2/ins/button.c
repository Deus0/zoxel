entity spawn_button(ecs *world, LayoutParentData canvas_data, LayoutParentData parent, ElementSpawnData element_data, SpawnTextData zext_data, SpawnButtonData button_data) {
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
    entity text = spawn_text_new(world, button_data.prefab_zext, e, int2_zero, float2_half, zext_data.font_size, zext_data.alignment, zext_data.margins, zext_data.text, zext_data.font_fill_color, zext_data.font_outline_color);
    zox_set(text, Layer2D, { element_data.layer + 1 });
    return e;
}
