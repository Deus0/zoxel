void button_event_load_cancel(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    //zox_geter(player, ElementLinks, elements);
    //find_array_element_with_tag(elements, MenuLoad, load_menu);
    entity canvas = zox_getv(player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    if (!menu) {
        zox_logw("MenuLoad menu not found");
    } else {
        zox_delete(menu);
    }
    spawn_main_menu(world, player, game_name);
}

void delay_spawn_menu_realm(ecs* world, entity player) {
    // zox_geter_value(player, GameLink, entity, game);
    // zox_geter_value(game, RealmLink, entity, realm);
    spawn_menu_realm(world, player);
}

void button_event_load_confirm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity clicked = event.clicked;
    zox_geter_value(player, GameLink, entity, game);
    zox_geter(clicked, FolderPath, path);
    // zox_geter(player, ElementLinks, elements);
    // find_array_element_with_tag(elements, MenuLoad, menu);
    entity canvas = zox_getv(player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
    if (!menu) {
        zox_log_error("UI [MenuLoad] Not Found");
        return;
    }
    zox_delete(menu);
    zox_logv("Save Realm Path [%s]", path->value);
    // TODO: Spawn a realm here and window for it
    entity realm = spawn_realm(world, prefab_realm);
    zox_set(game, RealmLink, { realm });
    zox_set_ptr(realm, FolderPath, path->value);
    load2_realm(path->value, "seed.dat", &realm_save);
    set_noise_seed(realm_save.seed);
    zox_set(realm, Seed, { realm_save.seed });
    delay_event(world, &delay_spawn_menu_realm, player, 0.01);
}

entity spawn_menu_load(ecs *world, entity player) {
    // Sizing
    int header_font_size = 16* ui_scale;
    byte list_font_size = 8 * ui_scale;
    // more data
    const char* header_label = "Which";
    int elements_count = 0;
    byte visible_count = 4;
    SpawnListElement elements[max_settings + 1];
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
    entity e = spawn_window_list(world, prefab_window, player, header_label, header_font_size, list_font_size, (ClickEvent) { &button_event_load_cancel }, 1, 0, 0, zox_alignment_centre, byte2_single(4), NULL, elements, elements_count, visible_count).x;
    zox_name("menu_load");
    zox_add_tag(e, MenuLoad);
    zox_add_tag(e, NavigationWindow);
    return e;
}
