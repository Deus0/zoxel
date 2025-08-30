extern entity prefab_window_users;

SpawnWindowUsers get_default_spawn_window_users_data(
    ecs *world,
    const entity prefab,
    const entity character,
    const entity canvas,
    const int2 canvas_size
) {
    const byte header_font_size = 26; // * zox_ui_scale;
    const byte header_margins = 6; // * zox_ui_scale;
    const byte header_height = header_font_size + header_margins * 2;
    const float2 anchor = float2_half;
    const int2 position = position;
    const byte2 grid_size = byte2_single(4);
    const byte2 grid_padding = byte2_single(6); // * zox_ui_scale);
    const int grid_margins = 16; // * zox_ui_scale;
    const int frame_size = default_frame_size; // * zox_ui_scale;
    const int icon_size = default_icon_size; // * zox_ui_scale;
    const int2 size = (int2) {
        grid_padding.x + (frame_size + grid_padding.x) * grid_size.x + grid_margins * 2,
        grid_padding.y + (frame_size + grid_padding.y) * grid_size.y + grid_margins * 2 + header_height
    };
    entity prefab_frame_ = prefab_frame;
    if (zox_has(prefab, FramePrefabLink)) {
        prefab_frame_ = zox_get_value(prefab, FramePrefabLink)
        // zox_log("+ prefab frame found! %s\n", zox_get_name(prefab_frame_))
    } else {
        zox_log_error("prefab frame failed! %s", zox_get_name(prefab));
    }
    SpawnWindowUsers data = {
        .canvas = {
            .e = canvas,
            .size = canvas_size
        },
        .parent = {
            .e = canvas,
            .position = int2_half(canvas_size),
            .size = canvas_size,
        },
        .element = {
            .prefab = prefab_window_users,
            .position = int2_zero,
            .size = size,
            .anchor = anchor
        },
        .header = {
            .prefab_zext = prefab_zext,
            .is_close_button = 1,
            .margins = header_margins
        },
        .header_zext = {
            .text = "Users",
            .font_size = header_font_size,
            .font_thickness = 4,
            .font_fill_color = header_font_fill,
            .font_outline_color = header_font_outline
        },
        .frame = {
            .prefab = prefab_frame_,
            .texture = {
                .fill_color = default_fill_color_frame,
                .outline_color = default_outline_color_frame
            }
        },
        .icon = {
            .prefab = prefab_icon,
            .texture = {
                .fill_color = default_fill_color_icon,
                .outline_color = default_outline_color_icon
            },
            .size = icon_size,
            .texture_size = int2_single(default_icon_texture_size)
        },
        .window = {
            .grid_size = grid_size,
            .grid_padding = grid_padding,
            .icon_size = frame_size,
            .character = character,
            .prefab_header = prefab_header,
            // .prefab_header_zext = prefab_zext,
        },
    };
    return data;
}