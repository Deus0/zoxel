entity spawn_elementbar2_front(ecs *world, entity canvas, entity parent, int2 parent_position, int2 parent_size, byte layer, byte render_disabled, color front_color) {
    byte margin = 3;
    int2 size = (int2) {
        parent_size.x - margin * 2,
        parent_size.y - margin * 2
    };
    zox_instance(prefab_elementbar2D_front);
    zox_name("elementbar2D_front");
    zox_set(e, FillColor, { front_color });
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
        layer
    );
    return e;
}
