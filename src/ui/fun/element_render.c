void set_children_render_disabled(ecs *world, entity e, byte disabled) {
    if (zox_has(e, RenderDisabled)) {
        zox_muter(e, RenderDisabled, render_disabled);
        render_disabled->value = disabled;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            set_children_render_disabled(world, e2, disabled);
        }
    }
}
