extern entity spawn_menu_load(ecs*, entity);

void button_event_menu_realm_cancel(ecs *world, ClickEventData event) {

    entity player = event.clicker;

    zox_geter(player, ElementLinks, elements);

    find_array_element_with_tag(elements, MenuRealm, menu);
    if (!menu) {
        zox_log_error("MenuRealm not found");
        return;
    }

    zox_delete(menu);

    // Delete Realm
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_delete(realm);
    zox_set(game, RealmLink, { 0 });

    spawn_menu_load(world, player);
}

void button_event_menu_realm_delete(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity clicked = event.clicked;

    zox_geter(player, ElementLinks, elements);
    // zox_geter_value(player, GameLink, entity, game);
    // zox_geter_value(game, RealmLink, entity, realm);

    find_array_element_with_tag(elements, MenuRealm, menu);
    if (!menu) {
        zox_log_error("main menu not found");
        return;
    }

    zox_delete(menu);

    // TODO: Confirm Screen

    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_log("Deleting Realm (%s)", zox_get_name(realm));


    // Destroys Realm
    zox_delete(realm);
    zox_set(game, RealmLink, { 0 });

    spawn_menu_load(world, player);
}

void button_event_menu_realm_confirm(ecs *world, ClickEventData event) {

    entity player = event.clicker;
    entity clicked = event.clicked;

    zox_geter(player, ElementLinks, elements);
    zox_geter_value(player, GameLink, entity, game);

    find_array_element_with_tag(elements, MenuRealm, menu);
    if (!menu) {
        zox_log_error("main menu not found");
        return;
    }

    zox_delete(menu);

    // start game
    zox_set(game, GameStateTarget, { zox_game_load_start });
}

entity spawn_menu_realm(ecs *world, entity player) {

    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);

    // Sizing
    int header_font_size = 16* ui_scale;
    byte list_font_size = 8 * ui_scale;

    // more data
    const char* header_label = "Realm";
    int elements_count = 0;
    byte visible_count = 4;
    SpawnListElement elements[max_settings + 1];

    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_label,
        .text = "Seed", // zox_get_name(realm),
    };

    zox_geter_value(realm, Seed, double, seed);
    char sseed[32];
    snprintf(sseed, sizeof(sseed), "%.0f", seed);

    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_label,
        .text = sseed,
    };

    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_button,
        .text = "Delete",
        .on_click = { &button_event_menu_realm_delete },
    };

    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_button,
        .text = "Confirm",
        .on_click = { &button_event_menu_realm_confirm },
    };

    entity e = spawn_window_list(
        world,
        prefab_menu_game,
        player,
        header_label,
        header_font_size,
        elements,
        elements_count,
        visible_count,
        list_font_size,
        (ClickEvent) { &button_event_menu_realm_cancel },
        1
    );
    zox_name("menu_realm");
    zox_add_tag(e, MenuRealm);
    zox_add_tag(e, NavigationWindow);

    return e;
}
