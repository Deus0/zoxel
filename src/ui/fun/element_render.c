void set_children_render_disabled(ecs *world, entity e, byte disabled) {
    if (zox_has(e, RenderDisabled)) {
        zox_muter(e, RenderDisabled, render_disabled);
        render_disabled->value = disabled;
    }
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        set_children_render_disabled(world, child, disabled);
    }
}
