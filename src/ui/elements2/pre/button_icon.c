entity spawn_prefab_button_icon(
ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("button_icon");
    zox_add_tag(e, Button);
    // transform
    zox_prefab_set(e, Children, { 0 });
    // texture
    set_frame_texture_type(
        world,
        e, default_fill_color,
        default_outline_color,
        default_button_corner,
        default_button_frame_thickness
    );

    //  interactable
    zox_add_tag(e, ClickMakeSound);
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { zox_select_state_none });
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });

    return e;
}
