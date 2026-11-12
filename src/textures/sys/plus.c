// A plus (+) shape, ie for spending stat points
void generate_texture_plus(
    color* data,
    int2 size,
    color fill,
    color outline,
    byte thickness)
{
    const color empty = color_clear;
    // Pass 1: two bars crossing at the center
    const int bar_width = size.x / 3 > 1 ? size.x / 3 : 1;
    const int bar_height = size.y / 3 > 1 ? size.y / 3 : 1;
    const int bar_x = (size.x - bar_width) / 2;
    const int bar_y = (size.y - bar_height) / 2;
    int index = 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            byte in_vertical =
                x >= bar_x &&
                x < bar_x + bar_width;
            byte in_horizontal =
                y >= bar_y &&
                y < bar_y + bar_height;
            data[index++] = (in_vertical || in_horizontal) ? fill : empty;
        }
    }
    // Pass 2: generate outline
    generate_texture_shape_outline(data, size, fill, outline, thickness);
}

void plus_texture_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TextureSize);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineColor);
    zox_sys_in(OutlineThickness);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, trigger);
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineColor, outline);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_o(TextureData, data);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        resize_TextureData(data, size->value.x * size->value.y);
        generate_texture_plus(
            data->value,
            size->value,
            fill->value,
            outline->value,
            thickness->value);
        zox_add(e, TextureDirty);
        if (dbg_log) {
            zox_log("Plus Texture Generated [%s] Size [%ix%i] Thickness [%i]",
                zox_get_name(e),
                size->value.x,
                size->value.y,
                thickness->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(plus_texture_system);
