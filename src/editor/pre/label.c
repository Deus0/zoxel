const color debug_color_outline = { 0, 255, 0, 255 }; // { 76, 66, 133, 255 };
const color debug_color_fill = { 0, 255, 0, 255 }; // { 132, 177, 212, 255 };

entity spawn_prefab_game_debug_label(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_add(e, GameDebugLabel);
    zox_prefab_add(e, DebugLabelData);
    zox_add(e, EditorElement);
    prefab_set_mesh2D_vertices(world, e, square_vertices_top_right_aligned, 4);
    return e;
}
