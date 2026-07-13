zox_sys2(FontTextureSystem) {
    float2 point_padding = font_point_padding;
    zox_change_check();
    zox_sys_world();
    // todo: link each zigel to fontstyle's font
    entity zox_font_style = get_font_style_using();
    if (!zox_font_style) {
        return;
    }
    uint font_children_capacity = 256;
    entity fchildren[font_children_capacity];
    uint fchildren_length = zox_get_children(world, zox_font_style, fchildren, font_children_capacity);
    if (!fchildren_length) {
        zox_log("font_style_children is NULL");
        return;
    }
    byte is_use_shapes = zox_has(zox_font_style, TTFFontStyle);
    zox_sys_begin();
    zox_sys_in(ZigelIndex);
    zox_sys_in(FillColor);
    zox_sys_in(SecondaryColor);
    zox_sys_in(TextureSize);
    zox_sys_in(FontThickness);
    zox_sys_in(FontOutlineThickness);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ZigelIndex, zindex);
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, color_variable);
        zox_sys_i(SecondaryColor, secondary_color);
        zox_sys_i(FontThickness, thickness);
        zox_sys_i(FontOutlineThickness, outline_thickness);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(TextureData, data);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        if (zindex->value >= fchildren_length) {
            zox_loge("Font Index out of bounds [%i] [%i]", zindex->value, fchildren_length);
            continue;
        }
        // get font based on zigel index
        entity font = fchildren[zindex->value];
        int length = size->value.x * size->value.y;
        if (length <= 0 || !thickness->value || !zox_valid(font)) {
            resize_TextureData(data, 0);
            dirty->value = zox_dirty_trigger;
            continue;
        }
        zox_geter(font, FontData, fontData);
        resize_TextureData(data, length);
        generate_font_texture(data->value, size->value, fontData, secondary_color->value, color_variable->value, is_use_shapes, thickness->value, outline_thickness->value, point_padding);
        dirty->value = zox_dirty_trigger;
        generate->value = zox_generate_texture_end;
#ifdef zoxel_debug_zigel_updates
        zox_log("Zigel font is updating [%lu]\n", it->entities[i])
#endif
    }
} zox_sys_end(FontTextureSystem);
