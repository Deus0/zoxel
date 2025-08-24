entity spawn_scrollbar(
    ecs *world,
    entity parent,
    entity canvas,
    int2 position,
    byte layer,
    int2 parent_size,
    int width,
    int scrollbar_margins,
    int visible_elements,
    int total_elements,
    byte visible
) {
    float2 anchor = (float2) { 1.0f, 0.5f };
    int2 size = (int2) { width, parent_size.y };
    int2 margins = (int2) { scrollbar_margins, 0 };
    int height = (int) parent_size.y * (float_min(1, (float) visible_elements / (float) total_elements));
    if (visible_elements == 0) {
        height = parent_size.y;
    }

    zox_instance(prefab_scrollbar);
    zox_name("scrollbar");
    initialize_element(
        world,
        e,
        parent,
        canvas,
        position,
        size,
        size,
        anchor,
        layer,
        float2_zero,
        int2_zero
    );
    zox_set(e, ElementMargins, { margins });
    zox_set(e, RenderDisabled, { !visible });

    Children children = { 0 };
    const entity handle = spawn_scrollbar_handle(
        world,
        e,
        canvas,
        (int2) { 0, (parent_size.y - height) / 2 }, // top of scroll
        float2_half,
        layer + 1,
        (int2) { width, height },
        parent_size,
        visible
    );
    add_to_Children(&children, handle);
    zox_set_ptr(e, Children, children);
    //zox_log("- scrollbar height set to: [%i] out of [%i]", height, parent_pixel_size.y)
    // zox_log("- scrollbar showing [%i] out of [%i]", visible_elements, total_elements);
    return e;
}
