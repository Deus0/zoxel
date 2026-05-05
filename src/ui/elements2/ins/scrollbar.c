entity spawn_scrollbar(ecs *world, entity parent, entity canvas, int2 position, byte layer, int2 parent_size, int width, int scrollbar_margins, int visible_elements, int total_elements, byte visible, entity scrollview) {

    float2 anchor = (float2) { 1.0f, 0.5f };
    int2 size = (int2) { width, parent_size.y };
    int2 margins = (int2) { scrollbar_margins, 0 };
    int handle_height = !visible_elements ? parent_size.y : (int) parent_size.y * (float_min(1, (float) visible_elements / (float) total_elements));
    // top of scroll
    int2 handle_position = (int2) { 0, (parent_size.y - handle_height) / 2 };

    zox_instance(prefab_scrollbar);
    zox_name("scrollbar");
    initialize_element(world, e, parent, canvas, position, size, size, anchor, layer);

    zox_set(e, BonusLayer2, { 1 }); // increment it above the body of the window
    zox_set(e, Color, { button_fill });
    zox_set(e, OutlineColor, { button_outline });
    zox_set(e, ElementMargins, { margins });
    zox_set(e, RenderDisabled, { !visible });

    // Children children = { 0 };

    entity handle = spawn_scrollbar_handle(world, prefab_scrollbar_front, e, canvas, handle_position, float2_half, layer + 1, (int2) { width, handle_height }, parent_size, visible);
    zox_set(handle, ScrollviewLink, { scrollview });
    zox_set_parent(world, handle, e);
    // add_to_Children(&children, handle);
    // zox_set_ptr(e, Children, children);
    //zox_log("- scrollbar height set to: [%i] out of [%i]", height, parent_pixel_size.y)
    // zox_log("- scrollbar showing [%i] out of [%i]", visible_elements, total_elements);

    return e;
}
