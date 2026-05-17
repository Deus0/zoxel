// TODO: Clean this up, make a general taskbar ui
extern void on_closed_taskbar_window(ecs*, ClickEventData);

void set_frame_texture_from_data(ecs *world, entity frame, entity icon, entity data) {
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

SpawnWindowUsers get_default_datagrid_data(ecs *world, entity prefab, entity character, entity canvas, int2 canvas_size) {
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
    /*entity prefab_frame_ = prefab_frame;
    if (zox_has(prefab, FramePrefabLink)) {
        prefab_frame_ = zox_get_value(prefab, FramePrefabLink);
    } else {
        zox_log_error("prefab frame failed! %s", zox_get_name(prefab));
    }*/
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
            .prefab = prefab_window,
            .anchor = anchor
        },
        .header = {
            .prefab_zext = prefab_zext,
            .is_close_button = 1
        },
        .header_zext = header_text_data,
        .frame = {
            // .prefab = prefab_frame_,
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

entity spawn_datagrid2(ecs *world, SpawnWindowUsers data, FrameTextureData window_texture, entity prefab_frame, entity prefab_icon, entity prefab_label, byte label_font_size, byte selected, const entity* udata, uint count) {
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
    entity frames[count];
    for (int j = data.window.grid_size.y - 1; j >= 0; j--) {
        if (array_index >= count) {
            break;
        }
        for (int i = 0; i < data.window.grid_size.x; i++) {
            if (array_index >= count) {
                break;
            }
            entity dat = udata[item_index];
            item_index++;
            entity3 frame_spawn = spawn_frame(world, prefab_frame, prefab_icon, prefab_label, grid, position, frame_size, icon_size, label_font_size, array_index);
            // NOTE: Atm this is what connects user data textures
            zox_set(frame_spawn.x, DataLink, { dat });
            zox_set(frame_spawn.y, DataLink, { dat });
            if (frame_spawn.z) {
                zox_set(frame_spawn.z, DataLink, { dat });
            }
            set_frame_texture_from_data(world, frame_spawn.x, frame_spawn.y, dat);
            frames[array_index] = frame_spawn.x;
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

entity spawn_datagrid(ecs* world, entity prefab, entity prefab_frame, entity prefab_icon, entity prefab_label, byte label_font_size, entity canvas, entity character, entity id, const char* header, color fill, color outline) {
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    SpawnWindowUsers data = get_default_datagrid_data(world, prefab, character, canvas, canvas_size);
    data.header_zext.text = header;
    data.element.prefab = prefab;
    data.frame.texture.fill_color = fill;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };
    entity datas[layouts2_children_capacity];
    uint dlength = zox_get_children_by_id(world, character, datas, layouts2_children_capacity, id);
    return spawn_datagrid2(world, data, texture, prefab_frame, prefab_icon, prefab_label, label_font_size, 0, datas, dlength);
}
