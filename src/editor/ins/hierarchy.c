// the header's p: shows prefabs too (a realm's are its children), or hides them
void hierarchy_prefabs_toggle(ecs* world, ClickEventData event) {
    const entity window = zox_get_parent_by_id(world, event.clicked, zox_id(HierarchyUI));
    const entity list = window ? get_editor_window_list(world, window) : 0;
    if (!list) {
        return;
    }
    const byte shown = !zox_has(list, HierarchyPrefabs);
    if (shown) {
        zox_add(list, HierarchyPrefabs);
    } else {
        zox_remove(list, HierarchyPrefabs);
    }
    zox_setv(event.clicked, FillColor, shown ? editor_hover_fill : editor_button_fill);
    if (zox_has(event.clicked, GenerateTexture)) {
        zox_setv(event.clicked, GenerateTexture, zox_generate_texture_run);    // shows it's on
    }
    zox_add(list, Dirty);
}

// The hierarchy window, on the left, listing root's entity tree (see hierarchy_spawn_event)
entity spawn_window_hierarchy(
    ecs *world,
    entity canvas,
    entity player,
    entity root)
{
    (void) player;
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int font_size = 12;
    const entity4 e4 = spawn_editor_window(world, prefab_hierarchy, canvas, "Hierarchy", dock_left,
        (int2) { canvas_size.x / 5, canvas_size.y }, font_size, editor_icon_hierarchy);
    const entity window = e4.x;
    const entity list = e4.w;
    zox_set_name_spawned(world, window, "hierarchy");
    zox_add(window, HierarchyUI);
    zox_setv(window, ElementFontSize, font_size);
    zox_setv(window, Target, root);
    // one row is active (highlighted) at a time
    zox_add(list, ActiveSingle);
    zox_add(list, VirtualList);
    const entity prefabs = spawn_button(world, prefab_clickable, e4.y, "p", int2_zero, (int2) { 20, 20 },
        (float2) { 1, 0.5f }, zox_alignment_centre, 12, byte2_single(0), header_fill, header_outline,
        header_font_fill, header_font_outline).x;
    zox_add(prefabs, HeaderButton);
    zox_setv(prefabs, ClickEvent, hierarchy_prefabs_toggle);
    hierarchy_list_open = list;
    // Use the list's event so its components are ready when a deferred spawn flushes.
    zox_add(list, Dirty);
    return window;
}
