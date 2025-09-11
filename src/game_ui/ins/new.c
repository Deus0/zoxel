void on_confirmed_new_realm(
    ecs *world,
    const ClickEventData event
) {
    entity player = event.clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuNewRealm, menu);
    if (menu) {
        zox_delete(menu);
    }

    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_geter_value(realm, Seed, long int, seed);

    char home_path[max_path_characters];
    get_home_directory(home_path, sizeof(home_path));

    char* game_path = join_paths(home_path, game_name);
    zox_log("game_path [%s]", game_path);
    create_new_directory(game_path);

    char text[64];
    sprintf(text, "%lu", (seed));

    char* save_dir = join_paths(game_path, text);
    free(game_path);
    zox_log("new realm save_dir [%s]", save_dir);
    create_new_directory(save_dir);

    SaveGamePath save_game_path = { };
    size_t len = strlen(save_dir);
    if (len >= 512) len = 512 - 1;
    memcpy(save_game_path.value, save_dir, len);
    save_game_path.value[len] = '\0';
    zox_set_ptr(realm, SaveGamePath, save_game_path); // ->value

    zox_log("confirm new realm [%s] [%lu]", game_name, seed);
    realm_save.seed = seed;

    save2_realm(save_dir, "seed.dat", &realm_save);
    free(save_dir);

    zox_set(game, GameStateTarget, { zox_game_load_start });
}

void on_cancelled_new_realm(
    ecs *world,
    const ClickEventData event
) {
    entity player = event.clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuNewRealm, menu);
    if (menu) {
        zox_delete(menu);
    }
    zox_log("canceled new realm");
    // zox_geter_value(player, CanvasLink, entity, canvas);
    spawn_main_menu(world, player, game_name);
}

entity spawn_menu_new_realm(
    ecs *world,
    const entity player,
    lint seed
) {
    zox_geter_value(player, CanvasLink, entity, canvas);

    // more data
    const char* header_label = "New Realm";
    const int max_labels = max_settings;
    const byte layer = 1;
    const byte font_size = 32;

    // # Window #
    LayoutParentData canvas_data = {
        .e = canvas,
        .size = zox_gett_value(canvas, LayoutSize),
    };
    ElementSpawnData window_element_data = {
        .prefab = prefab_window,
        .position = (int2) { 0, 0 },
        .size = (int2) { 520, 580 },
        .anchor = float2_half,
        .layer = layer,
    };
    LayoutParentData window_parent_data = {
        .e = canvas_data.e,
        .size = canvas_data.size,
        // .position = int2_half(canvas_data.size),
    };
    SpawnWindow2 window_data = {
        .header_text = header_label,
        .header_font_size = 32,
        .header_padding = (byte2) { 8, 4 },
        .is_scrollbar = 0,
    };

    Children children = (Children) { 0 };
    window_data.children = &children;
    const entity e = spawn_window2(
        world,
        canvas_data,
        window_parent_data,
        window_element_data,
        window_data,
        (ClickEvent) { &on_cancelled_new_realm },
        1
    );
    zox_add_tag(e, MenuNewRealm);

    // # List #
    SpawnListElement elements[max_labels];
    int elements_count = 0;

    elements[elements_count++] = (SpawnListElement) {
        .text = "Enter",
        .on_click = { &on_confirmed_new_realm },
    };

    elements[elements_count++] = (SpawnListElement) {
        .type = 0,  // 0 is button or label for now
        .text = "Seed",
    };

    char seed_label[32];
    sprintf(seed_label, "[%lu]", seed);
    elements[elements_count++] = (SpawnListElement) {
        .type = 0,  // 0 is button or label for now
        .text = seed_label,
    };

    /*elements[elements_count++] = (SpawnListElement) {
        .text = "Exit",
        .on_click = { &on_cancelled_new_realm },
    };*/

    const byte visible_count = elements_count;

    LayoutParentData list_parent_data = {
        .e = e,
        .size = window_element_data.size,
        .position = window_element_data.position_in_canvas,
    };
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .position = (int2) { 0, -25 },
        .size = (int2) { window_element_data.size.x, window_element_data.size.y - 50 },
        .anchor = float2_half,
        .layer = layer + 1,
    };
    SpawnList ui_list_data = (SpawnList) {
        .elements = elements,
        .count = elements_count,
        .visible_count = visible_count,
        .font_size = font_size,
        .fill = (color) { 0, 0, 0, 0 },
        .outline = (color) { 0, 255, 255, 55 },
        .slider_height = 64,
        .slider_padding = 64,
        .button_padding = (byte2) { 18, 12 },
        .padding = (byte2) { 18, 18 },
        .margins = (byte2) { 18, 18 },
    };
    const entity list = spawn_list(
        world,
        canvas_data,
        list_parent_data,
        list_element_data,
        ui_list_data
    );
    add_to_Children(window_data.children, list);
    zox_set_ptr(e, Children, children);

    zox_muter(player, ElementLinks, pelements);
    add_to_ElementLinks(pelements, e);
    zox_set(e, ElementHolder, { player });

    // return our window
    return e;
}

void button_event_new_game(
    ecs *world,
    const ClickEventData event
) {
    entity player = event.clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuMain, menu);
    if (menu) {
        zox_delete(menu);
    } else {
        zox_log_error("Could not find main menu.");
    }
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    lint seed = get_unique_time_seed();
    set_noise_seed(seed);
    zox_set(realm, Seed, { seed });
    spawn_menu_new_realm(world, player, seed);
}