entity spawn_prefab_grid(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("grid");
    zox_prefab_set(e, GridDirty, { zox_dirty_trigger });
    zox_prefab_set(e, GridPadding, { byte2_zero });
    zox_prefab_set(e, GridMargins, { byte2_zero });
    // body stuff
    zox_add_tag(e, Selectable);
    zox_add_tag(e, WindowRaycastTarget);
    zox_add_tag(e, FixToLayout);
    add_frame_texture_type(world, e, window_fill, window_outline, default_button_corner, default_button_frame_thickness);
    return e;
}
