entity spawn_elementbar2D_front(
    ecs *world,
    const entity canvas,
    const entity parent,
    const int2 parent_position,
    const int2 parent_size,
    const int2 canvas_size,
    const byte layer,
    const byte render_disabled,
    const color front_color
) {
    const byte margin = 3;
    const int2 size = (int2) {
        parent_size.x - margin * 2,
        parent_size.y - margin * 2
    };

    zox_instance(prefab_elementbar2D_front);
    zox_name("elementbar2D_front");
    zox_set(e, Color, { front_color });
    zox_set(e, RenderDisabled, { render_disabled });
    zox_set(e, MeshDirty, { mesh_state_trigger });

    initialize_element(
        world,
        e,
        parent,
        canvas,
        int2_zero,
        size,
        int2_one,
        (float2) { 0, 0.5f },
        layer,
        float2_zero,
        int2_zero
    );

    return e;
}
