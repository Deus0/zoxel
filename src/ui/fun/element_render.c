void set_children_render_disabled(ecs *world, entity e, byte disabled) {

    if (zox_has(e, RenderDisabled)) {
        zox_muter(e, RenderDisabled, render_disabled);
        render_disabled->value = disabled;
    }

    if (zox_has(e, Children)) {
        zox_geter(e, Children, children);
        if (children) {
            for (int i = 0; i < children->length; i++) {
                entity child = children->value[i];

                set_children_render_disabled(world, child, disabled);
            }
        }
    }
}
