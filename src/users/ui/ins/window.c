// TODO: Rename/Refactor to WindowIcons
entity spawn_window_users(
    ecs *world,
    SpawnWindowUsers data,
    FrameTextureData window_texture,
    byte selected,
    entity3* spawns
) {
    const entity character = data.window.character;
    if (!zox_valid(character) || !zox_has(character, ElementLinks)) {
        zox_log_error("invalid character in spawn icons window.");
        return 0;
    }
    if (!zox_has_id(character, data.window.user_links_id)) {
        zox_log_error("Character [%lu] has no [%s], cannot spawn ui", character, zox_get_name(data.window.user_links_id))
        return 0;
    }
    const UserLinks *user_data = zox_get_id(character, data.window.user_links_id);

    const byte body_layer = data.element.layer + 1;
    const byte icon_layer = body_layer + 1;
    const byte is_header = data.window.prefab_header != 0;
    int2 position = data.element.position;
    byte header_height = 0;
    if (is_header) {
        header_height = data.header_zext.font_size + data.header.margins;
    }

    zox_instance(data.element.prefab);
    zox_set_unique_name(e, data.header_zext.text);
    initialize_element(
        world,
        e,
        data.parent.e,
        data.canvas.e,
        position,
        data.element.size,
        data.element.size,
        data.element.anchor,
        data.element.layer,
        float2_zero,
        int2_zero
    );
    set_window_bounds_to_canvas(
        world,
        e,
        data.canvas.size,
        data.element.size,
        data.element.anchor
    );

    const int user_datas_count = user_data->length;
    const int grid_elements_count = user_datas_count; // data.window.grid_size.x * data.window.grid_size.y

    const int children_length = 1 + is_header;
    Children children = (Children) { 0 };
    initialize_Children(&children, children_length);
    if (children.length != children_length) {
        zox_log_error("Failed to iniitalize children.");
        return e;
    }

    int2 header_size = int2_zero;
    if (is_header) {
        const byte header_layer = data.element.layer + 1; // 3;
        const float2 header_anchor = (float2) {
            0.5f,
            1.0f
        };
        const int2 header_position = (int2) {
            0,
            -header_height / 2

        };
        header_size = (int2) {
            data.element.size.x,
            header_height
        };
        // zox_set(e, HeaderHeight, { header_size.y })
        // todo: pass more of t this in from top
        SpawnHeader spawnHeader = {
            .canvas = data.canvas,
            .parent = {
                .e = e,
                .size = data.element.size
            },
            .element = {
                .prefab = data.window.prefab_header,
                .layer = header_layer,
                .anchor = header_anchor,
                .position = header_position,
                .size = header_size
             },
            .zext = data.header_zext,
            .header = data.header
        };
        children.value[0] = spawn_header2(world, &spawnHeader);
    }

    // todo:
    //  - fill in element data
    //  - set window prefab as invisible (since im only showing header/panel combo)

    // spawn body
    int2 body_size = int2_sub(data.element.size, (int2) { 0, header_size.y });
    int2 body_position = (int2) { 0, -header_size.y / 2 };
    ElementSpawn spawn_body_data = {
        .canvas = data.canvas,
        .parent = {
            .e = e,
            .size = data.element.size
        },
        .element = {
            .prefab = prefab_body,
            .layer = body_layer,
            .anchor = float2_half,
            .position = body_position,
            .size = body_size
        },
        .texture = window_texture,
    };
    const entity body = spawn_element(world, &spawn_body_data);
    zox_set_unique_name(body, "window_users_body");
    children.value[is_header] = body;

    Children body_children = (Children) { 0 };
    initialize_Children(&body_children, grid_elements_count);

    int item_index = 0;
    int array_index = 0;
    const byte active_states = zox_has(data.frame.prefab, ActiveState);

    for (int j = data.window.grid_size.y - 1; j >= 0; j--) {
        if (array_index >= body_children.length) {
            break;
        }
        for (int i = 0; i < data.window.grid_size.x; i++) {
            if (array_index >= body_children.length) {
                break;
            }
            const int2 position = {
                (int) ((i - (data.window.grid_size.x / 2.0f) + 0.5f) * (data.window.icon_size + data.window.grid_padding.x)),
                (int) ((j - (data.window.grid_size.y / 2.0f) + 0.5f) * (data.window.icon_size + data.window.grid_padding.y))
            };
            SpawnFrame frame_data = {
                .canvas = data.canvas,
                .icon = data.icon,
                .parent = {
                    .e = body,
                    .position = spawn_body_data.element.position_in_canvas,
                    .size = spawn_body_data.element.size
                },
                .element = {
                    .prefab = data.frame.prefab,
                    .position = position,
                    .size = int2_single(data.window.icon_size),
                    .layer = icon_layer,
                    .anchor = float2_half
                },
                .texture = data.frame.texture
            };
            frame_data.icon.index = array_index;
            const entity user_data_element = user_data->value[item_index];

            entity3 frame_spawn = spawn_frame_user(
                world,
                frame_data,
                user_data_element
            );
            body_children.value[array_index] = frame_spawn.x;
            if (spawns) {
                spawns[array_index] = frame_spawn;
            }

            array_index++;
            item_index++;
        }
    }

    if (active_states) {
        entity selected_frame = body_children.value[selected];
        zox_set(selected_frame, ActiveState, { 1 });
        zox_set(selected_frame, ActiveStateDirty, { zox_dirty_trigger });
    }

    zox_set_ptr(body, Children, body_children);
    zox_set_ptr(e, Children, children);

    // add to characters element links and link to character
    zox_muter(character, ElementLinks, elementLinks);
    add_to_ElementLinks(elementLinks, e);
    zox_set(e, ElementHolder, { character });

    return e;
}
