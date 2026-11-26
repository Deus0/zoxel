// its rows spawned a frame on (InspectorDirty's observer): spawned from a click (a system's, deferred) its list's
//  components weren't set when the window's add fired, no rows (tst editor, clicked as a mouse does)
void inspector_dirty_delayed(ecs* world, entity window) {
    if (zox_valid(window)) {
        zox_add(window, InspectorDirty);
    }
}

// The inspector window, on the right, listing target's components (see inspector_spawn_system)
entity spawn_inspector(
    ecs *world,
    entity canvas,
    entity player,
    entity target)
{
    (void) player;
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int font_size = 12;
    const entity window = spawn_editor_window(world, prefab_inspector, canvas, "Inspector", dock_right,
        (int2) { canvas_size.x / 5, canvas_size.y }, font_size).x;
    zox_set_name_spawned(world, window, "inspector");
    zox_add(window, InspectorUI);
    zox_setv(window, FontSize, font_size);
    zox_setv(window, Target, target);
    delay_event(world, &inspector_dirty_delayed, window, 0);
    return window;
}
