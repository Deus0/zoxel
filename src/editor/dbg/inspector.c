void toggle_inspector(ecs *world, int32_t keycode) {
    // our logic stuff
    if (keycode != zox_key_u) {
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    entity e = zox_get_child_by_id(world, canvas, zox_id(InspectorUI));
    // entity e = get_canvas_window(world, canvas, zox_window_inspector);
    if (e) {
        zox_log("Deleting Editor Inspector");
        zox_delete(e);
        spawn_sound_from_file_index(world, prefab_sound, 1);
    } else {
        zox_log("Spawning Editor Inspector");
        spawn_inspector(world, canvas, player, canvas);
        spawn_sound_from_file_index(world, prefab_sound, 0);
    }
}

void toggle_inspector_target(ecs* world, entity player, entity target) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(target)) {
        return;
    }
    entity e = 0;   // target inspector
    /*entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, canvas, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity e2 = children[j];*/
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2) || !zox_has(e2, InspectorUI)) {
                continue;
            }
            // check target
            entity otarget = zox_getv(e2, EntityTarget);
            if (otarget == target) {
                e = e2;
                break;
            }
        }
    }
    if (e) {
        zox_log("Deleting Editor Inspector for [%s]", zox_get_name(target));
        zox_delete(e);
        spawn_sound_from_file_index(world, prefab_sound, 1);
    } else {
        zox_log("Spawning Editor Inspector for [%s]", zox_get_name(target));
        spawn_inspector(world, canvas, player, target);
        spawn_sound_from_file_index(world, prefab_sound, 0);
    }
}

void toggle_inspector_player(ecs* world, int32_t keycode) {
    // our logic stuff
    if (keycode != zox_key_i && keycode != zox_key_o) {
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity target = keycode == zox_key_i ? player : zox_has(player, CharacterLink) ? zox_gett_value(player, CharacterLink) : 0;
    if (!zox_valid(target)) {
        return;
    }
    toggle_inspector_target(world, player, target);
}
