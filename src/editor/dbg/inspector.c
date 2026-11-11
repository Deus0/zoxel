// u: toggles the inspector, of the canvas
void toggle_inspector(ecs *world, int32_t keycode) {
    if (keycode != zox_key_u) {
        return;
    }
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return;
    }
    const entity inspector = get_canvas_inspector(world, canvas);
    if (inspector) {
        zox_delete(inspector);
        play_editor_toggle_sound(world, 0);
    } else {
        spawn_inspector(world, canvas, dbg_player, canvas);
        play_editor_toggle_sound(world, 1);
    }
}

// toggles an inspector of target: closes the one inspecting it, otherwise opens one
void toggle_inspector_target(
    ecs* world,
    entity player,
    entity target)
{
    const entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas) || !zox_valid(target)) {
        return;
    }
    entity inspector = 0;
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count && !inspector; j++) {
            const entity e = it2.entities[j];
            if (zox_has(e, InspectorUI) && zox_getv(e, EntityTarget) == target) {
                inspector = e;
            }
        }
    }
    if (inspector) {
        zox_delete(inspector);
        play_editor_toggle_sound(world, 0);
    } else {
        spawn_inspector(world, canvas, player, target);
        play_editor_toggle_sound(world, 1);
    }
}

// i: toggles inspecting the player, o: its character (not hooked to keys by default)
void toggle_inspector_player(ecs* world, int32_t keycode) {
    if (keycode != zox_key_i && keycode != zox_key_o) {
        return;
    }
    const entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    const entity target = keycode == zox_key_i ? player : zox_get_link(world, player, CharacterLink);
    toggle_inspector_target(world, player, target);
}
