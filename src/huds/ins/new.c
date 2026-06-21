void on_confirmed_new_realm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuNewRealm, menu);
    if (menu) {
        zox_delete(menu);
    }
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_geter_value(realm, Seed, lint, seed);
    char home_path[max_path_characters];
    get_home_directory(home_path, sizeof(home_path));
    char* game_path = join_paths(home_path, game_name);
    create_new_directory(game_path);
    char text[64];
    sprintf(text, "%lu", (seed));
    char* save_dir = join_paths(game_path, text);
    create_new_directory(save_dir);
    FolderPath save_game_path = { };
    size_t len = strlen(save_dir);
    if (len >= 512) len = 512 - 1;
    memcpy(save_game_path.value, save_dir, len);
    save_game_path.value[len] = '\0';
    zox_set_ptr(realm, FolderPath, save_game_path); // ->value
    zox_set(game, GameStateTarget, { zox_game_load_start });
    // zox_logv("game_path [%s]", game_path);
    // zox_log("confirm new realm [%s] [%lu]", game_name, seed);
    realm_save.seed = seed;
    save2_realm(save_dir, "seed.dat", &realm_save);
    // Verbose Log
    zox_logv("Created new Saves Directory [%s]", save_dir);
    free(game_path);
    free(save_dir);
}

void on_cancelled_new_realm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    zox_geter(player, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuNewRealm, menu);
    if (menu) {
        zox_delete(menu);
    }
    // zox_log("canceled new realm");
    // Delete Realm
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_delete(realm);
    zox_set(game, RealmLink, { 0 });
    spawn_main_menu(world, player, game_name);
}

entity spawn_menu_new_realm(ecs *world, entity player) {
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        return 0;
    }
    zox_geter_value(realm, Seed, lint, seed);
    int2 size = (int2) { 128 * ui_scale, 96 * ui_scale };
    byte list_font_size = 8 * ui_scale;
    byte2 button_padding = (byte2) { 6 * ui_scale, 4 * ui_scale };
    byte2 list_padding = (byte2) { 8 * ui_scale, 8 * ui_scale };
    // more data
    zox_geter_value(player, CanvasLink, entity, canvas);
    const char* header_label = "Where"; // "New Realm";
    int max_labels = max_settings;
    byte layer = 1;
    // # Window #
    LayoutParentData canvas_data = { .e = canvas };
    // LayoutParentData window_parent_data = { .e = canvas };
    entity3 e3 = spawn_window(world, prefab_window, prefab_body, header_label, canvas, int2_zero, size, float2_half, &on_cancelled_new_realm);
    entity e = e3.x;
    entity body = e3.z;
    zox_add_tag(e, MenuNewRealm);
    zox_add_tag(e, NavigationWindow);
    // # List #
    SpawnListElement elements[max_labels];
    int elements_count = 0;
    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_label,
        .text = "Seed",
    };
    char seed_label[32];
    sprintf(seed_label, "[%lu]", seed);
    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_label,
        .text = seed_label,
    };
    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_button,
        .text = "Enter",
        .on_click = { &on_confirmed_new_realm },
    };
    byte visible_count = elements_count;
    LayoutParentData list_parent_data = { .e = body };
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = size,
        .anchor = float2_half,
    };
    SpawnList ui_list_data = (SpawnList) {
        .elements = elements,
        .count = elements_count,
        .visible_count = visible_count,
        .font_size = list_font_size,
        .fill = (color) { 0, 0, 0, 0 },
        .outline = (color) { 0, 255, 255, 55 },
        .slider_height = 64,
        .slider_padding = 64,
        .button_padding = button_padding,
        .padding = list_padding,
        .margins = list_padding,
    };
    entity list = spawn_list(world, canvas_data, list_parent_data, list_element_data, ui_list_data, zox_alignment_centre, NULL);
    // zox_set_parent(world, list, e);
    zox_muter(player, ElementLinks, pelements);
    add_to_ElementLinks(pelements, e);
    zox_set(e, ElementHolder, { player });
    return e;
}

void delay_spawn_menu_new_realm(ecs* world, entity player) {
    spawn_menu_new_realm(world, player);
}

void button_event_new_game(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_getv(player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity main_menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
    if (zox_valid(main_menu)) {
        close_ui_related_tooltip(world, main_menu);
        zox_delete(main_menu);
    }
    entity game = zox_get_parent(world, player);
    // Spawn a new Realm!
    entity realm = spawn_realm(world, prefab_realm);
    zox_set(game, RealmLink, { realm });
    lint seed = get_unique_time_seed();
    set_noise_seed(seed);
    zox_set(realm, Seed, { seed });
    delay_event(world, &delay_spawn_menu_new_realm, player, 0.01);
}
