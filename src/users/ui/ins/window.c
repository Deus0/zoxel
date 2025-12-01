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
        zox_log_error("Character [%lu] has no [%s], cannot spawn ui", character, zox_get_name(data.window.user_links_id));
        return 0;
    }
    const UserLinks *user_data = zox_get_id(character, data.window.user_links_id);

    const byte is_header = data.window.prefab_header != 0;
    int2 position = data.element.position;
    byte header_height = is_header ? data.header_zext.font_size + data.header_zext.margins.y * 2 : 0;

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
        data.element.layer
    );
    set_window_bounds_to_canvas(
        world,
        e,
        data.canvas.size,
        data.element.size,
        data.element.anchor
    );

    const int user_datas_count = user_data->length;
    const int grid_elements_count = user_datas_count;

    const int children_length = 1 + is_header;
    Children children = (Children) { 0 };
    initialize_Children(&children, children_length);
    if (children.length != children_length) {
        zox_log_error("Failed to iniitalize children.");
        return e;
    }

    if (is_header) {
        const LayoutParentData e_parent_data = { .e = e };
        ElementSpawnData header_element_data = {
            .prefab = data.window.prefab_header,
            .layer = data.element.layer + 1,
            .anchor = (float2) { 0.5f, 1 },
            .position = (int2) { 0, -header_height / 2 },
            .size = (int2) {
                data.element.size.x,
                header_height
            },
        };

        const entity header = spawn_header3(
            world,
            data.canvas,
            e_parent_data,
            header_element_data,
            data.header_zext,
            data.header,
            (ClickEvent) { &on_closed_taskbar_window }
        );
        children.value[0] = header;
    }

    // spawn body
    const byte body_layer = data.element.layer + 1;
    int2 grid_size = int2_sub(data.element.size, (int2) { 0, header_height });
    ElementSpawn grid_data = {
        .texture = window_texture,
        .canvas = data.canvas,
        .parent = { .e = e },
        .element = {
            .prefab = prefab_grid,
            .layer = body_layer,
            .anchor = float2_half,
            .position = (int2) { 0, -header_height / 2 },
            .size = grid_size,
        },
    };
    const entity grid = spawn_element(world, &grid_data);
    zox_set_unique_name(grid, "window_users_grid");
    children.value[is_header] = grid;
    zox_set(grid, GridSize, { data.window.grid_size });
    zox_set(grid, GridPadding, { data.window.grid_padding });
    zox_set(grid, GridMargins, { data.window.grid_margins });

    Children body_children = (Children) { 0 };
    initialize_Children(&body_children, grid_elements_count);

    const byte icon_layer = body_layer + 1;
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
            SpawnFrame frame_data = {
                .canvas = data.canvas,
                .texture = data.frame.texture,
                .icon = data.icon,
                .parent = { .e = grid },
                .element = {
                    .prefab = data.frame.prefab,
                    .size = int2_single(data.window.icon_size),
                    .layer = icon_layer,
                    .anchor = float2_half,
                },
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

    zox_set_ptr(grid, Children, body_children);
    zox_set_ptr(e, Children, children);

    // add to characters element links and link to character
    zox_muter(character, ElementLinks, elementLinks);
    add_to_ElementLinks(elementLinks, e);
    zox_set(e, ElementHolder, { character });

    return e;
}
