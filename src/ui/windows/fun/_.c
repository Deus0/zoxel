entity get_canvas_window(ecs* world, entity e, byte find_type) {
    if (!zox_valid(e)) {
        return 0;
    }

    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
    /*zox_geter(e, Children, children);
    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];*/

        if (!zox_valid(child) || !zox_has(child, WindowType)) {
            continue;
        }

        zox_geter_value(child, WindowType, byte, window_type);

        if (window_type == find_type) {
            return child;
        }
    }

    return 0;
}
