// The hierarchy window, on the left, listing root's entity tree (see hierarchy_spawn_system)
entity spawn_window_hierarchy(
    ecs *world,
    entity canvas,
    entity player,
    entity root)
{
    (void) player;
    const int font_size = 12;
    const entity4 e4 = spawn_editor_window(world, prefab_hierarchy, canvas, "Hierarchy", -1, font_size);
    const entity window = e4.x;
    const entity list = e4.w;
    zox_set_name_spawned(world, window, "hierarchy");
    zox_add(window, HierarchyUI);
    zox_setv(window, ElementFontSize, font_size);
    zox_setv(window, EntityTarget, root);
    // one row is active (highlighted) at a time
    zox_add(list, ActiveSingle);
    return window;
}
