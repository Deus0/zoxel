// TODO: Rename/Refactor to WindowIcons

entity spawn_window_users(ecs *world, SpawnWindowUsers data, FrameTextureData window_texture, byte selected, entity3* spawns, const entity* udata, int udata_length) {
    entity character = data.window.character;
    if (!zox_valid(character) || !zox_has(character, ElementLinks)) {
        zox_log_error("invalid character in spawn icons window.");
        return 0;
    }
    byte is_header = data.window.prefab_header != 0;
    int2 position = data.element.position;
    byte header_height = is_header ? data.header_zext.font_size + data.header_zext.margins.y * 2 : 0;
    // Spawns window here spawn_window_users
    zox_instance(data.element.prefab);
    zox_set_unique_name(e, data.header_zext.text);
    initialize_element(world, e, data.parent.e, data.canvas.e, position, data.element.size, data.element.size, data.element.anchor, data.element.layer);
    set_window_bounds_to_canvas(world, e, data.canvas.size, data.element.size, data.element.anchor);
    int grid_elements_count = udata_length;
    if (is_header) {
        LayoutParentData e_parent_data = { .e = e };
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
        data.header_zext.font_resolution = data.header_zext.font_size;
        entity header = spawn_header3(world, data.canvas, e_parent_data, header_element_data, data.header_zext, data.header, (ClickEvent) { &on_closed_taskbar_window });
        zox_set_parent(world, header, e);
        // children.value[0] = header;
    }
    // spawn body
    byte body_layer = data.element.layer + 1;
    int2 grid_size = int2_sub(data.element.size, (int2) { 0, header_height });
    int2 grid_position = (int2) { 0, -header_height / 2 };
    /*ElementSpawn grid_data = {
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
    };*/
    //entity grid = spawn_element(world, grid_data);
    entity grid = spawn_uic(world, prefab_grid, e, float2_half, grid_position, grid_size, grid_size, window_texture.fill_color, window_texture.outline_color);
    zox_set_unique_name(grid, "window_users_grid");
    zox_set(grid, GridSize, { data.window.grid_size });
    zox_set(grid, GridPadding, { data.window.grid_padding });
    zox_set(grid, GridMargins, { data.window.grid_margins });
    // zox_set_parent(world, grid, e);
    entity body_children[grid_elements_count];
    byte icon_layer = body_layer + 1;
    int item_index = 0;
    int array_index = 0;
    byte active_states = zox_has(data.frame.prefab, ActiveState);
    for (int j = data.window.grid_size.y - 1; j >= 0; j--) {
        if (array_index >= grid_elements_count) {
            break;
        }
        for (int i = 0; i < data.window.grid_size.x; i++) {
            if (array_index >= grid_elements_count) {
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
            entity user_data_element = udata[item_index];

            entity3 frame_spawn = spawn_frame_user(world, frame_data, user_data_element);
            body_children[array_index] = frame_spawn.x;
            // zox_set_parent(world, frame_spawn.x, grid);
            if (spawns) {
                spawns[array_index] = frame_spawn;
            }
            array_index++;
            item_index++;
        }
    }

    if (active_states) {
        if (selected >= grid_elements_count) {
            zox_logw("selected [%i] out of bounds [%i]", selected, grid_elements_count);
            selected = grid_elements_count - 1;
        }
        if (!grid_elements_count) {
            zox_logw("no children to select");
        } else {
            entity selected_frame = body_children[selected];
            zox_set(selected_frame, ActiveState, { 1 });
            zox_set(selected_frame, ActiveStateDirty, { zox_dirty_trigger });
        }
    }

    // add to characters element links and link to character
    zox_muter(character, ElementLinks, elementLinks);
    add_to_ElementLinks(elementLinks, e);
    zox_set(e, ElementHolder, { character });

    return e;
}


entity spawn_window_users_id(ecs *world, SpawnWindowUsers data, FrameTextureData window_texture, byte selected, entity3* spawns) {
    entity character = data.window.character;

    if (!zox_valid(character) || !zox_has(character, ElementLinks)) {
        zox_log_error("invalid character in spawn icons window.");
        return 0;
    }

    if (!zox_has_id(character, data.window.user_links_id)) {
        zox_log_error("Character [%lu] has no [%s], cannot spawn ui", character, zox_get_name(data.window.user_links_id));
        return 0;
    }

    const UserLinks *udata = zox_get_id(character, data.window.user_links_id);

    return spawn_window_users(world, data, window_texture, selected, spawns, udata->value, udata->length);
}
