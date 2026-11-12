// The inspector window, on the right, listing target's components (see inspector_spawn_system)
entity spawn_inspector(
    ecs *world,
    entity canvas,
    entity player,
    entity target)
{
    (void) player;
    const int font_size = 12;
    const entity window = spawn_editor_window(world, prefab_inspector, canvas, "Inspector", 1, font_size).x;
    zox_set_name_spawned(world, window, "inspector");
    zox_add(window, InspectorUI);
    zox_setv(window, FontSize, font_size);
    zox_setv(window, EntityTarget, target);
    return window;
}
