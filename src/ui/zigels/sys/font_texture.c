void FontTextureSystem(iter *it) {
    const color air_color = color_null;
    const float2 point_padding = font_point_padding;
    // const byte default_font_outline = 1; // 4
    zox_change_check()
    zox_sys_world()
    // todo: link each zigel to fontstyle's font
    entity zox_font_style = get_font_style_using();
    if (!zox_font_style || !zox_has(zox_font_style, Children)) {
        return;
    }
    zox_geter(zox_font_style, Children, font_style_children)
    if (!font_style_children) {
        zox_log("! font_style_children is NULL\n")
        return;
    }
    const byte is_use_shapes = zox_has(zox_font_style, TTFFontStyle);

    zox_sys_begin();
    zox_sys_in(GenerateTexture);
    zox_sys_in(ZigelIndex);
    zox_sys_in(Color);
    zox_sys_in(SecondaryColor);
    zox_sys_in(TextureSize);
    zox_sys_in(FontThickness);
    zox_sys_in(FontOutlineThickness);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(GenerateTexture, generate);
        zox_sys_i(ZigelIndex, zigelIndex);
        zox_sys_i(TextureSize, size);
        zox_sys_i(Color, color_variable);
        zox_sys_i(SecondaryColor, secondaryColor);
        zox_sys_i(FontThickness, fontThickness);
        zox_sys_i(FontOutlineThickness, fontOutlineThickness);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(TextureData, data);

        if (generate->value != zox_dirty_active || zigelIndex->value >= font_styles_length) {
            continue;
        }
        // get font based on zigel index
        const entity font = font_style_children->value[zigelIndex->value];
        int length = size->value.x * size->value.y;
        if (length <= 0 || fontThickness->value == 0 || !zox_valid(font)) {
            resize_TextureData(data, 0);
            /*for (int j = 0; j < length; j++) {
                textureData->value[j] = air_color;
            }*/
            dirty->value = zox_dirty_trigger;
            continue;
        }
        zox_geter(font, FontData, fontData)
        resize_TextureData(data, length);
        generate_font_texture(
            data->value,
            size->value,
            fontData,
            secondaryColor->value,
            color_variable->value,
            is_use_shapes,
            fontThickness->value,
            fontOutlineThickness->value,
            point_padding
        );
        dirty->value = zox_dirty_trigger;
#ifdef zoxel_debug_zigel_updates
        zox_log("     > zigel font is updating [%lu]\n", it->entities[i])
#endif
    }
} zoxd_system(FontTextureSystem)