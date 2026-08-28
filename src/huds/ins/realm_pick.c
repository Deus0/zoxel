void button_event_load_cancel(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_getv(player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    if (!menu) {
        zox_logw("MenuLoad menu not found");
    } else {
        zox_delete(menu);
    }
    spawn_main_menu(
        world,
        player,
        game_name);
}

void delay_spawn_menu_realm(ecs* world, entity player) {
    spawn_menu_realm(world, player);
}

void button_event_load_confirm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity clicked = event.clicked;
    zox_geter_value(player, GameLink, entity, game);
    zox_geter(clicked, FolderPath, path);
    entity canvas = zox_getv(player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    if (!menu) {
        zox_log_error("UI [MenuLoad] Not Found");
        return;
    }
    zox_delete(menu);
    zox_logv("Save Realm Path [%s]", path->value);
    SaveRealm realm_save;
    load_file_struct(
        path->value,
        "seed.dat",
        &realm_save,
        sizeof(SaveRealm));
    // TODO: Spawn a realm here and window for it
    entity realm = spawn_realm(
        world,
        game,
        realm_save.seed);
    zox_set_ptr(realm, FolderPath, path->value);
    delay_event(
        world,
        &delay_spawn_menu_realm,
        player,
        0.01);
}

entity spawn_menu_load(ecs *world, entity player) {
    const char* header_label = "Which";
    byte visible_count = 4;
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = zox_huds_header_font_size * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 padding = byte2_single(zox_huds_list_padding * ui_scale);
    SpawnListElement elements[huds_max_list_elements + 1];
    int elements_count = 0;
    char **save_dirs;
    byte saves_count;
    // TODO: Load folder names here
    if (!get_save_games(game_name, &save_dirs, &saves_count)) {
        for (byte i = 0; i < saves_count; i++) {
            char* folder = get_folder_name(save_dirs[i]);
            elements[elements_count++] = (SpawnListElement) {
                .text = folder,
                .save_path = save_dirs[i],
                .on_click = { &button_event_load_confirm },
            };
            zox_logv("Save Dir: %s - folder %s", save_dirs[i], folder);
        }
        free(save_dirs);
    }
    entity e = spawn_window_list(
        world,
        prefab_window,
        player,
        header_label,
        header_font_size,
        list_font_size,
        (ClickEvent) { &button_event_load_cancel },
        1,
        0,
        0,
        zox_alignment_centre,
        window_anchor,
        padding,
        NULL,
        elements,
        elements_count,
        visible_count).x;
    zox_name("menu_load");
    zox_add(e, MenuLoad);
    zox_add(e, NavigationWindow);
    return e;
}
