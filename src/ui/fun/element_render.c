void set_children_render_disabled(
    ecs *world,
    const entity e,
    const byte disabled
) {
    if (zox_has(e, RenderDisabled)) {
        zox_muter(e, RenderDisabled, render_disabled);
        render_disabled->value = disabled;
    }
    if (zox_has(e, Children)) {
        zox_geter(e, Children, children);
        if (children) {
            for (int i = 0; i < children->length; i++) {
                set_children_render_disabled(
                    world,
                    children->value[i],
                    disabled
                );
            }
        }
    }
}
