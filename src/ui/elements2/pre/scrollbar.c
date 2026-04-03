entity spawn_prefab_scrollbar(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("scrollbar");

    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { zox_select_state_none });

    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });

    zox_add_tag(e, Scrollbar);
    zox_prefab_set(e, Children, { 0 });
    zox_prefab_set(e, ElementMargins, { int2_zero });

    zox_add_tag(e, FixToLayout);
    add_frame_texture_type(world, e, scrollbar_color, default_outline_color, default_button_corner, default_button_frame_thickness);

    return e;
}
