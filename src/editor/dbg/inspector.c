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

    entity e = get_canvas_window(world, canvas, zox_window_inspector);

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
    zox_geter(canvas, Children, children);
    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];

        if (!zox_valid(child) || !zox_has(child, WindowType)) {
            continue;
        }

        zox_geter_value(child, WindowType, byte, window_type);
        if (window_type == zox_window_inspector) {
            // check target
            zox_geter_value(child, EntityTarget, entity, otarget);
            if (otarget == target) {
                e = child;
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
