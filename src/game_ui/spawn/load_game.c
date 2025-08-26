void button_event_load_cancel(
    ecs *world,
    const ClickEventData *event
) {
    entity player = event->clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuLoad, menu);
    if (!menu) {
        zox_log_error("MenuLoad menu not found")
        return;
    }
    zox_delete(menu);
    spawn_main_menu(world, player, game_name);
}


void button_event_load_confirm(
    ecs *world,
    const ClickEventData* event
) {
    entity player = event->clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuLoad, menu);
    if (!menu) {
        zox_log_error("main menu not found")
        return;
    }
    zox_delete(menu);
    const entity game = zox_get_value(player, GameLink);
    const entity realm = zox_get_value(game, RealmLink);

    entity clicked = event->clicked;
    zox_geter(clicked, SaveGamePath, path);
    zox_log("Path [%s]", path->value);
    zox_set_ptr(realm, SaveGamePath, path->value);

    // todo: spawn realm data like voxels/stats/skills when starting new game
    // load2_realm(game_name, "seed.dat", &realm_save);
    load2_realm(path->value, "seed.dat", &realm_save);

    set_noise_seed(realm_save.seed);
    zox_set(realm, Seed, { realm_save.seed });
    zox_set(game, GameStateTarget, { zox_game_load });
}


entity spawn_menu_load(
    ecs *world,
    const entity player
) {
    // more data
    const char* header_label = "Load";
    const int header_font_size = 72;
    const byte visible_count = 4; // 12;
    const byte layer = 1;
    const byte list_font_size = 32;

    SpawnListElement elements[max_settings + 1];
    int elements_count = 0;

    // TODO: Load folder names here
    char **save_dirs;
    byte saves_count;
    if (get_save_games(game_name, &save_dirs, &saves_count) == 0) {
        for (byte i = 0; i < saves_count; i++) {
            char* folder = get_folder_name(save_dirs[i]);
            elements[elements_count++] = (SpawnListElement) {
                .text = folder,
                .save_path = save_dirs[i],
                .on_click = { &button_event_load_confirm },
            };
            zox_log("Dir: %s - folder %s", save_dirs[i], folder);
        }
        free(save_dirs);
        // free_dirs(save_dirs, saves_count);
    }

    /*elements[elements_count++] = (SpawnListElement) {
        .text = "Continue",
        .on_click = { &button_event_load_confirm },
    };*/

    elements[elements_count++] = (SpawnListElement) {
        .text = "Return",
        .on_click = { &button_event_load_cancel },
    };

    const entity e = spawn_window_list(
        world,
        player,
        header_label,
        header_font_size,
        elements,
        elements_count,
        visible_count,
        list_font_size
    );
    zox_add_tag(e, MenuLoad);
    zox_name("menu_load");

    return e;
}