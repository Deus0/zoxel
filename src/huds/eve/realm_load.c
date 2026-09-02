void button_event_menu_realm_cancel(
    ecs *world,
    ClickEventData event)
{
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, Canvas);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuRealm));
    if (!menu) {
        zox_loge("MenuRealm not found");
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

void button_event_menu_realm_delete(
    ecs *world,
    ClickEventData event)
{
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, Canvas);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuRealm));
    if (!menu) {
        zox_log_error("main menu not found");
        return;
    }
    zox_delete(menu);
    // TODO: Confirm Screen
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    zox_geter(realm, FolderPath, realm_path);
    // zox_log("Deleting Realm (%s: %s", zox_get_name(realm), realm_path);
    if (delete_dir(realm_path->value)) {
        zox_logv("Success Deleting Realm: %s", realm_path);
    } else {
        zox_log_error("Error Deleting Realm: %s", realm_path);
    }
    // Destroys Realm
    zox_delete(realm);
    zox_set(game, RealmLink, { 0 });
    spawn_menu_load(world, player);
}

void button_event_menu_realm_confirm(
    ecs *world,
    ClickEventData event)
{
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, Canvas);
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuRealm));
    zox_geter_value(player, GameLink, entity, game);
    // find_array_element_with_tag(elements, MenuRealm, menu);
    if (!menu) {
        zox_log_error("main menu not found");
        return;
    }
    zox_delete(menu);
    // start game
    zox_set(game, GameStateTarget, { zox_game_load_start });
}
