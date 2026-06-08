entity2 spawn_slider(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, SpawnSliderData slider_data, color fill, color outline, byte font_size, color font_fill, color font_outline) {
    byte handle_width = slider_data.handle_width; // 8 * ui_scale;
    byte font_thickness = ui_scale;
    // spawn back part
    zox_instance(element_data.prefab);
    zox_name("slider");
    zox_set(e, SliderLabel, { slider_data.name });
    zox_set_parent(world, e, parent_data.e);
    zox_set(e, LayoutPosition, { element_data.position });
    zox_set(e, LayoutSize, { element_data.size });
    zox_set(e, Anchor, { element_data.anchor });
    zox_set(e, FillColor, { fill });
    zox_set(e, OutlineColor, { outline });
    // spawn handle
    float percent = clampf(slider_data.value, 0, 1);
    int handle_position_x = - element_data.size.x / 2 + handle_width / 2 + (int) ((element_data.size.x - handle_width) * percent);
    entity prefab_handle = slider_data.prefab_handle;
    int2 handle_position = (int2) { handle_position_x, 0 };
    int2 handle_size =  (int2) { handle_width, element_data.size.y };
    entity handle = spawn_handle(world, prefab_handle, e, element_data.size, handle_position, handle_size, float2_half, handle_fill, handle_outline, 0);
    zox_set(handle, SlideBounds, { slider_data.bounds });
    zox_set_parent(world, handle, e);
    spawn_text(world, prefab_text, e, int2_zero, float2_half, font_size, zox_alignment_centre, byte2_zero, slider_data.name, font_fill, font_outline);
    return (entity2) { e, handle };
}
