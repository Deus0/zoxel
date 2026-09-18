// just a panel with a label and a bar
// the bar can be clicked to reposition the handle as well
entity spawn_prefab_slider(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("slider");
    zox_add(e, SliderLabel);
    zox_add(e, FixToLayout);
    zox_setv(e, FrameCorner, default_button_corner);
    zox_setv(e, OutlineThickness, default_button_frame_thickness);
    zox_setv(e, FillColor, default_fill_color);
    zox_setv(e, OutlineColor, default_outline_color);
    return e;
}
