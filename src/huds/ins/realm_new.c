void on_confirmed_new_realm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, Canvas);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuNewRealm));
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
    SaveRealm realm_save;
    realm_save.seed = seed;
    save_file_struct(save_dir, "seed.dat", &realm_save, sizeof(SaveRealm));
    // Verbose Log
    zox_logv("Created new Saves Directory [%s]", save_dir);
    free(game_path);
    free(save_dir);
}

void on_cancelled_new_realm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, Canvas);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuNewRealm));
    if (menu) {
        zox_delete(menu);
    }
    // Delete Realm
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_delete(realm);
    zox_setv(game, RealmLink, 0);
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
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = zox_huds_header_font_size * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 list_padding = byte2_single(zox_huds_list_padding * ui_scale);
    byte2 button_padding = (byte2) { 6 * ui_scale, 4 * ui_scale };
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    // more data
    entity canvas = zox_get_link(world, player, Canvas);
    const char* header_label = "Where"; // "New Realm";
    int max_labels = huds_max_list_elements;
    // # Window #
    LayoutParentData canvas_data = { .e = canvas };
    entity3 e3 = spawn_window(world, prefab_window, prefab_body, header_label, canvas, int2_zero, size, window_anchor, header_font_size, header_padding, &on_cancelled_new_realm);
    entity e = e3.x;
    entity body = e3.z;
    zox_add(e, MenuNewRealm);
    zox_add(e, NavigationWindow);
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
    spawn_list(world, canvas_data, list_parent_data, list_element_data, ui_list_data, zox_alignment_centre, NULL);
    return e;
}

void delay_spawn_menu_new_realm(ecs* world, entity player) {
    spawn_menu_new_realm(world, player);
}
