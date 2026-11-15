// Creates the new realms save folder (its realm.txt: seed, name), and starts loading the game
//  - name 0 gives it a name from its seed
void enter_new_realm(ecs *world, entity player, const char* name) {
    entity game = zox_get_link(world, player, GameLink);
    entity realm = zox_get_link(world, game, RealmLink);
    lint seed = zox_getv(realm, Seed);
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
    RealmInfo info = { .seed = (uint) seed, .last = (long) time(NULL) };
    if (name && name[0]) {
        snprintf(info.name, sizeof(info.name), "%s", name);
    } else {
        realm_default_name(info.seed, info.name, sizeof(info.name));
    }
    save_realm_info(save_dir, &info);
    realm_played_since = zox_current_time;
    // Verbose Log
    zox_logv("Created new Saves Directory [%s]", save_dir);
    free(game_path);
    free(save_dir);
}

// its two input fields, by their place in its list: the name, then the seed
static inline void menu_new_realm_fields(ecs* world, entity e, entity* name, entity* seed) {
    if (zox_has(e, TextInput)) {
        const int index = zox_has(e, ChildIndex) ? zox_getv(e, ChildIndex) : 0;
        if (!*name || (zox_has(*name, ChildIndex) && index < zox_getv(*name, ChildIndex))) {
            if (*name) {
                *seed = *name;
            }
            *name = e;
        } else {
            *seed = e;
        }
        return;
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            menu_new_realm_fields(world, it.entities[i], name, seed);
        }
    }
}

// the name typed for the realm entered next (a new seed respawns its realm first, a frame later)
char new_realm_name[realm_name_length];

void delay_enter_named_realm(ecs *world, entity player) {
    enter_new_realm(world, player, new_realm_name);
}

void on_confirmed_new_realm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuNewRealm));
    entity game = zox_get_link(world, player, GameLink);
    entity realm = zox_get_link(world, game, RealmLink);
    new_realm_name[0] = 0;
    uint seed = zox_valid(realm) ? (uint) zox_getv(realm, Seed) : 0;
    if (menu) {
        entity name_field = 0, seed_field = 0;
        menu_new_realm_fields(world, menu, &name_field, &seed_field);
        char* name = input_field_value(world, name_field);
        snprintf(new_realm_name, sizeof(new_realm_name), "%s", name);
        free(name);
        char* seed_text = input_field_value(world, seed_field);
        if (seed_text[0]) {
            seed = realm_seed_from_text(seed_text);
        }
        free(seed_text);
        input_field_unfocus(world);
        zox_delete(menu);
    }
    // a different seed typed: its realm made again with it, entered once it's linked
    if (zox_valid(realm) && seed != (uint) zox_getv(realm, Seed)) {
        zox_delete(realm);
        spawn_realm(world, game, seed);
        delay_event(world, &delay_enter_named_realm, player, 0.01);
        return;
    }
    enter_new_realm(world, player, new_realm_name);
}

// a new random seed in its seed field, and a name from it if the name was still the old seed's
void on_randomise_new_realm_seed(ecs *world, ClickEventData event) {
    entity canvas = zox_get_link(world, event.clicker, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuNewRealm));
    entity name_field = 0, seed_field = 0;
    menu_new_realm_fields(world, menu, &name_field, &seed_field);
    if (!seed_field) {
        return;
    }
    char* old_text = input_field_value(world, seed_field);
    char old_name[realm_name_length];
    realm_default_name(realm_seed_from_text(old_text), old_name, sizeof(old_name));
    free(old_text);
    const uint seed = get_unique_time_seed();
    char text[16];
    snprintf(text, sizeof(text), "%u", seed);
    input_field_set_value(world, seed_field, text);
    char* name = input_field_value(world, name_field);
    if (!strcmp(name, old_name)) {
        char new_name[realm_name_length];
        realm_default_name(seed, new_name, sizeof(new_name));
        input_field_set_value(world, name_field, new_name);
    }
    free(name);
}

void on_cancelled_new_realm(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuNewRealm));
    if (menu) {
        zox_delete(menu);
    }
    // Delete Realm
    entity game = zox_get_link(world, player, GameLink);
    entity realm = zox_get_link(world, game, RealmLink);
    zox_delete(realm);
    spawn_main_menu(
        world,
        player,
        game_name);
}

entity spawn_menu_new_realm(
    ecs *world,
    entity player)
{
    entity game = zox_get_link(world, player, GameLink);
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        return 0;
    }
    const lint seed = zox_getv(realm, Seed);
    const byte header_font_size = zox_huds_header_font_size * ui_scale;
    const byte list_font_size = zox_huds_element_font_size * ui_scale;
    // half the usual gap: its six rows fit a small screen (enter scrolled out of view at 460 pixels)
    const byte2 padding = byte2_single(zox_huds_list_padding * ui_scale / 2);
    // its name (one from its seed to start) and its seed, typed or randomised; the window fits its list
    char name[realm_name_length];
    realm_default_name((uint) seed, name, sizeof(name));
    char seed_text[16];
    snprintf(seed_text, sizeof(seed_text), "%u", (uint) seed);
    SpawnListElement elements[] = {
        { .type = list_element_type_label, .text = "Name" },
        { .type = list_element_type_input, .text = name },
        { .type = list_element_type_label, .text = "Seed" },
        { .type = list_element_type_input, .text = seed_text },
        { .type = list_element_type_button, .text = "Randomise", .on_click = { &on_randomise_new_realm_seed } },
        { .type = list_element_type_button, .text = "Enter", .on_click = { &on_confirmed_new_realm } },
    };
    const int elements_count = sizeof(elements) / sizeof(elements[0]);
    entity e = spawn_window_list(
        world,
        prefab_window,
        player,
        "Where",
        header_font_size,
        list_font_size,
        (ClickEvent) { &on_cancelled_new_realm },
        1,
        0,
        0,
        zox_alignment_centre,
        zox_huds_window_anchor,
        padding,
        NULL,
        elements,
        elements_count,
        elements_count).x;
    zox_add(e, MenuNewRealm);
    zox_add(e, NavigationWindow);
    return e;
}

void delay_spawn_menu_new_realm(ecs* world, entity player) {
    spawn_menu_new_realm(world, player);
}
