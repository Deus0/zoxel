static byte zox_tst_editor_realm_ready(ecs* world) {
    static int step;
    static entity spawned;
    zox_tst_progress(step);
    if (!step) {
        if (!zox_tst_ui_fixture_canvas(world)) {
            return 0;
        }
        toggle_hierarchy(world, zox_key_y);
        step++;
        return 0;
    }

    const entity canvas = get_editor_canvas(world);
    const entity bar = docks_find(world, canvas).w;
    const ordered_children menus = zox_get_ordered_children(world, bar);
    if (menus.length != 4) {
        zox_tst_waiting = "the Realm menu";
        return 0;
    }
    const entity menu = menus.value[3];
    const entity item = item_list_item_at(world, menu, 0);
    const entity label = zox_get_child_by_id(world, item, zox_id(TextLabel));
    const entity realm = zox_get_link(world, local_game, RealmLink);
    if (step == 1) {
        zox_tst_click(world, dbg_player, menu);
        zox_tst_check(!realm && is_zext(zox_get(label, TextData), "Spawn Realm"), "expected Spawn Realm before generation");
        zox_tst_editor_menu_item_click(world, item);
    } else if (step == 2) {
        zox_tst_waiting = "the editor realm to finish generating";
        if (!zox_valid(realm) || zox_getv(realm, GenerateRealm)) {
            return 0;
        }
        spawned = realm;
        zox_tst_check(local_realm == realm && is_zext(zox_get(label, TextData), "Dispose Realm"),
            "new realm did not switch the item to Dispose Realm");
        const entity hierarchy = zox_get_child_by_id(world, canvas, zox_id(HierarchyUI));
        HierarchyWalk walk = hierarchy_walk_begin(world, hierarchy, zox_tst_editor_list(world, hierarchy));
        walk.anchor = realm;
        hierarchy_walk(world, &walk);
        zox_tst_check(!zox_get_parent(world, realm) && walk.anchor_index >= 0, "hierarchy walk did not include the realm root");
        zox_tst_click(world, dbg_player, menu);
        zox_tst_editor_menu_item_click(world, item);
    } else {
        zox_tst_waiting = "the editor realm to be deleted";
        if (zox_valid(spawned)) {
            return 0;
        }
        zox_tst_check(!realm && !zox_valid(local_realm) && is_zext(zox_get(label, TextData), "Spawn Realm"),
            "disposing did not clear the realm and reset its label");
        zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
        return 1;
    }
    step++;
    return 0;
}
