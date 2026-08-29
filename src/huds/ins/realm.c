extern entity spawn_menu_load(ecs*, entity);
extern void button_event_menu_realm_cancel(ecs*, ClickEventData);
extern void button_event_menu_realm_delete(ecs*, ClickEventData);
extern void button_event_menu_realm_confirm(ecs*, ClickEventData);

entity spawn_menu_realm(
    ecs *world,
    entity player)
{
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    // Sizing
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = zox_huds_header_font_size * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 list_padding = byte2_single(zox_huds_list_padding * ui_scale);
    // more data
    const char* header_label = "Come";
    int elements_count = 0;
    byte visible_count = 4;
    SpawnListElement elements[huds_max_list_elements + 1];
    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_label,
        .text = "Seed",
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
        .text = "Forget",
        .on_click = { &button_event_menu_realm_delete },
    };
    elements[elements_count++] = (SpawnListElement) {
        .type = list_element_type_button,
        .text = "Enter",
        .on_click = { &button_event_menu_realm_confirm },
    };
    entity e = spawn_window_list(
        world,
        prefab_window,
        player,
        header_label,
        header_font_size,
        list_font_size,
        (ClickEvent) { &button_event_menu_realm_cancel },
        1,
        0,
        0,
        zox_alignment_centre,
        window_anchor,
        list_padding,
        NULL,
        elements,
        elements_count,
        visible_count).x;
    zox_name("menu_realm");
    zox_add(e, MenuRealm);
    zox_add(e, NavigationWindow);
    return e;
}
