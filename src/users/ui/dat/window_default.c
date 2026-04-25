extern entity prefab_window_users;

int2 calculate_grid_size(SpawnWindowUsersData data) {

    return (int2) {
        data.grid_padding.x
        + (data.icon_size + data.grid_padding.x) * data. grid_size.x
        + data.grid_margins.x * 2,
        data.grid_padding.y
        + (data.icon_size + data.grid_padding.y) * data.grid_size.y
        + data.grid_margins.y * 2
    };
}

int2 calculate_grid_window_size(SpawnWindowUsersData data, int header_height) {
    return (int2) {

        data.grid_padding.x * (data. grid_size.x - 1)
        + data.icon_size * data. grid_size.x
        + data.grid_margins.x * 2,

        data.grid_padding.y * (data. grid_size.y - 1)
        + data.icon_size * data. grid_size.y
        + data.grid_margins.y * 2
        + header_height

    };
}

SpawnWindowUsers get_default_spawn_window_users_data(ecs *world, entity prefab, entity character, entity canvas, int2 canvas_size) {
    // Sizing
    byte header_font_size = 6 * ui_scale;
    byte2 header_margins = (byte2) {
        4 * ui_scale,
        3 * ui_scale
    };
    byte2 grid_padding = byte2_single(2 * ui_scale);
    byte2 grid_margins = byte2_single(4 * ui_scale);
    int frame_size = (default_frame_size / 4) * ui_scale;
    int icon_size = (default_icon_size / 4) * ui_scale;
    byte header_font_thickness_s = header_font_thickness * ui_scale;
    byte header_fonto_thickness_s = header_font_thickness * ui_scale;

    // Misc
    byte2 grid_size = byte2_single(4);
    float2 anchor = float2_half;
    int2 position = position;

    entity prefab_frame_ = prefab_frame;
    if (zox_has(prefab, FramePrefabLink)) {
        prefab_frame_ = zox_get_value(prefab, FramePrefabLink);
    } else {
        zox_log_error("prefab frame failed! %s", zox_get_name(prefab));
    }
    SpawnTextData header_text_data = {
        .text = "Users",
        .font_size = header_font_size,
        .font_resolution = header_font_size, // header_font_resolution,
        .font_thickness = header_font_thickness_s,
        .font_outline_thickness = header_fonto_thickness_s,
        .font_fill_color = header_font_fill,
        .font_outline_color = header_font_outline,
        .margins = header_margins
    };
    SpawnWindowUsers data = {
        // NOTE: Need size here for the bounds
        // TODO: Make bounds set in system instead
        .canvas = { .e = canvas, .size = canvas_size },
        .parent = { .e = canvas }, //, .size = canvas_size },
        .element = {
            .prefab = prefab_window_users,
            .anchor = anchor
        },
        .header = {
            .prefab_zext = prefab_zext,
            .is_close_button = 1
        },
        .header_zext = header_text_data,
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
            .grid_margins = grid_margins,
            .icon_size = frame_size,
            .character = character,
            .prefab_header = prefab_header,
        },
    };

    byte header_height = header_text_data.font_size + header_text_data.margins.y * 2;
    data.element.size = calculate_grid_window_size(
        data.window,
        header_height
    );

    return data;
}
