// TODO: Rename/Refactor to WindowIcons
void set_icon_from_user_data(ecs *world, entity frame, entity icon, entity data) {
    if (!zox_valid(frame) || !zox_valid(icon)) {
        zox_log_error("invalid frame or e in icon setting")
        return;
    }
    zox_set(frame, DataLink, { data });
    zox_set(icon, DataLink, { data });
    entity texture = zox_valid(data) && zox_has(data, TextureLink) ? zox_gett_value(data, TextureLink) : 0;
    if (!texture) {
        if (zox_valid(data)) {
            texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
        } else {
            texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
        }
    }
    clone_texture_data(world, icon, texture);
}

entity spawn_window_users(ecs *world, SpawnWindowUsers data, FrameTextureData window_texture, byte selected, entity3* spawns, const entity* udata, uint count) {
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
    // int grid_elements_count = udata_length;
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
    }
    int2 grid_size = int2_sub(data.element.size, (int2) { 0, header_height });
    int2 grid_position = (int2) { 0, -header_height / 2 };
    entity grid = spawn_uic(world, prefab_grid, e, float2_half, grid_position, grid_size, grid_size, window_texture.fill_color, window_texture.outline_color);
    zox_set_unique_name(grid, "window_users_grid");
    zox_set(grid, GridSize, { data.window.grid_size });
    zox_set(grid, GridPadding, { data.window.grid_padding });
    zox_set(grid, GridMargins, { data.window.grid_margins });
    byte active_states = zox_has(data.frame.prefab, ActiveState);
    int2 frame_size = int2_single(data.window.icon_size);
    int2 icon_size =  int2_single(data.icon.size);
    int item_index = 0;
    int array_index = 0;
    byte label_font_size = 3 * ui_scale;
    entity frames[count];
    for (int j = data.window.grid_size.y - 1; j >= 0; j--) {
        if (array_index >= count) {
            break;
        }
        for (int i = 0; i < data.window.grid_size.x; i++) {
            if (array_index >= count) {
                break;
            }
            entity eudata = udata[item_index];
            item_index++;
            entity3 frame_spawn = spawn_frame(world, data.frame.prefab, data.icon.prefab, 0, grid, position, frame_size, icon_size, label_font_size, array_index);
            // NOTE: Atm this is what connects user data textures
            set_icon_from_user_data(world, frame_spawn.x, frame_spawn.y, eudata);
            frames[array_index] = frame_spawn.x;
            if (spawns) {
                spawns[array_index] = frame_spawn;
            }
            array_index++;
        }
    }
    if (active_states) {
        if (selected >= count) {
            zox_logw("selected [%i] out of bounds [%i]", selected, count);
            selected = count - 1;
        }
        if (!count) {
            zox_logw("no children to select");
        } else {
            entity selected_frame = frames[selected];
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
    const UserLinks* udata = zox_get_id(character, data.window.user_links_id);
    return spawn_window_users(world, data, window_texture, selected, spawns, udata->value, udata->length);
}
