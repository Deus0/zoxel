extern const color icon_overlay_fill_color;
extern const color icon_overlay_outline_color;

entity spawn_icon_overlay(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data) {
    zox_instance(element_data.prefab);
    zox_name("icon_overlay");
    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }
    zox_set(e, Color, { icon_overlay_fill_color });
    zox_set(e, OutlineColor, { icon_overlay_outline_color });
    // zox_set(e, RenderDisabled, { 1 })
    return e;
}