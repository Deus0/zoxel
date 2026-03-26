entity get_canvas_window(ecs* world, entity canvas, byte find_type) {
    if (!zox_valid(canvas) || !zox_has(canvas, Children)) {
        return 0;
    }

    zox_geter(canvas, Children, children);
    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];

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
