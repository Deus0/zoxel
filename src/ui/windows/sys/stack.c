// #define zox_debug_canvas_stack
/*static inline void set_recursively_layer(ecs* world, entity e, byte value) {
    if (!zox_has(e, Layer)) {
        return;
    }
    zox_muter(e, Layer, layer2);
    layer2->value = value;
    value++;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2)) {
                continue;
            }
            set_recursively_layer(world, e2, value);
        }
    }
}*/

// todo: implement localLayer's here for elements'
byte get_highest_layer(
    ecs *world,
    entity e,
    byte layer)
{
    if (!e) {
        return layer;
    }
    byte child_layer = layer + 1;
    byte highest_layer = layer;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            byte new_layer = get_highest_layer(
                world,
                e2,
                child_layer);
            if (new_layer > highest_layer) {
                highest_layer = new_layer;
            }
        }
    }
    return highest_layer;
}


byte get_highest_window_layer(ecs* world, entity e) {
    byte layer = 0;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_has(e2, Window) ||
                zox_has(e2, IgnoreCanvasStack)
            ) {
                continue;
            }
            byte window_layers = get_highest_layer(
                world,
                e2,
                1);
            if (window_layers > layer) {
                layer = window_layers;
            }
        }
    }
    return layer;
}

// Reorders windows on a stack, moves all previous ones up the stack when a new one is placed
// TODO: Just check children instead and use a CanvasDirty flag
zox_sys2(CanvasStackSystem) {
    const byte dbg_log = 0;
    const uint max_checks = 255;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(WindowToTop);
    zox_sys_out(WindowsLayers);
    zox_sys_out(WindowsCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(WindowToTop, add_window);
        zox_sys_o(WindowsLayers, layers);
        zox_sys_o(WindowsCount, count);
        if (!zox_valid(add_window->value) ||
            !zox_has(add_window->value, WindowLayer) ||
            zox_has(add_window->value, IgnoreCanvasStack)
        ) {
            add_window->value = 0;
            continue;
        }
        byte old_layer = zox_getv(add_window->value, WindowLayer);
        uint windows_count = zox_get_children_count(
            world,
            e);
        byte layers_per_window = get_highest_window_layer(world, e);
        // gett previous window layer of moving to top window
        // skip if: same window clicked as already on top
        if (windows_count == count->value && old_layer == windows_count) {
            add_window->value = 0;
            if (dbg_log) {
                zox_log(" > skipping stack refresh as same window as last selected", old_layer);
            }
            continue;
        }
        // set_recursively_layer(world, add->window, windows_count * layers_per_window);
        zox_setv(add_window->value, WindowLayer, windows_count);
        zox_add(add_window->value, WindowLayerDirty);
        layers->value = layers_per_window;
        byte old_windows_count = count->value;
        byte not_assigned_index = count->value + 1; // start on top of stack, but below latest
        count->value = windows_count;
        int_hashmap* windows = create_int_hashmap(windows_count);
        int_hashmap_add(windows, windows_count, add_window->value);
        if (dbg_log) {
            zox_log("Canvas Stack Refreshing [%i] New Window [%s]", windows_count, zox_getn(add_window->value));
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2) ||
                    !zox_has(e2, Window) ||
                    zox_has(e2, IgnoreCanvasStack))
                {
                    continue;
                }
                if (add_window->value == e2) {
                    continue;
                }
                byte child_window_layer = zox_getv(e2, WindowLayer);
                // if on top
                if (child_window_layer == 0) {
                    byte new_layer = not_assigned_index;
                    int checks = 0;
                    while (int_hashmap_has(windows, new_layer)) {
                        if (new_layer == 1) {
                            new_layer = old_windows_count;
                        } else {
                            new_layer--;
                        }
                        if (dbg_log) {
                            zox_log("Issue with index, lowering [%i]", new_layer);
                        }
                        checks++;
                        if (checks > max_checks) {
                            zox_log("FAILED CANVAS STACKING 1\n")
                            int_hashmap_dispose(windows);
                            add_window->value = 0;
                            return; // failure!
                        }
                    }
                    if (!int_hashmap_has(windows, new_layer)) {
                        int_hashmap_add(windows, new_layer, e2);
                    } else {
                        zox_log("Layer already stacked [%i]\n", new_layer)
                    }
                    zox_setv(e2, WindowLayer, new_layer);
                    zox_add(e2, WindowLayerDirty);
                    not_assigned_index++;
                    if (dbg_log) {
                        zox_log("   - [%i] setting new child WindowLayer [%ls]", new_layer, zox_getn(e2));
                    }
                } else if (old_layer != 0 && child_window_layer > old_layer) {
                    byte new_layer = child_window_layer - 1;
                    if (new_layer > windows_count - 1) {
                        new_layer = windows_count - 1;
                    }
                    int checks = 0;
                    while (int_hashmap_has(windows, new_layer)) {
                        if (new_layer == 1) {
                            new_layer = old_windows_count;
                        } else {
                            new_layer--;
                        }
                        if (dbg_log) {
                            zox_log("Issue with index, lowering [%i]", new_layer);
                        }
                        checks++;
                        if (checks > max_checks) {
                            zox_logw("FAILED CANVAS STACKING 2\n")
                            int_hashmap_dispose(windows);
                            add_window->value = 0;
                            return; // failure!
                        }
                    }
                    if (!int_hashmap_has(windows, new_layer)) {
                        int_hashmap_add(windows, new_layer, e2);
                    } else {
                        zox_log("! layer already stacked [%i]", new_layer);
                    }
                    zox_setv(e2, WindowLayer, new_layer);
                    zox_add(e2, WindowLayerDirty);
                    if (dbg_log) {
                        zox_log("   - [%i] decreasing WindowLayer [%s]", new_layer, zox_getn(e2));
                    }
                } else {
                    if (int_hashmap_has(windows, child_window_layer)) {
                        byte new_layer = child_window_layer;
                        int checks = 0;
                        while (int_hashmap_has(windows, new_layer)) {
                            if (new_layer == 1) {
                                new_layer = old_windows_count;
                            } else {
                                new_layer--;
                            }
                            if (dbg_log) {
                                zox_log("   ! issue with index, lowering [%i]", new_layer);
                            }
                            checks++;
                            if (checks > max_checks) {
                                zox_logw("FAILED CANVAS STACKING 3");
                                int_hashmap_dispose(windows);
                                add_window->value = 0;
                                return; // failure!
                            }
                        }
                        if (!int_hashmap_has(windows, new_layer)) {
                            int_hashmap_add(windows, new_layer, e2);
                        } else {
                            zox_logw("Layer already stacked [%i]", new_layer);
                        }
                        zox_setv(e2, WindowLayer, new_layer);
                        zox_add(e2, WindowLayerDirty);
                        if (dbg_log) {
                            zox_log("   - [%i] anew WindowLayer [%s]", new_layer, zox_getn(e2));
                        }
                    } else {
                        int_hashmap_add(windows, child_window_layer, e2);
                        if (dbg_log) {
                            zox_log("   - [%i] keeping WindowLayer [%s]", child_window_layer, zox_getn(e2));
                        }
                    }
                }
            }
        }
        int_hashmap_dispose(windows);
        add_window->value = 0;
    }
} zox_sys_end(CanvasStackSystem);
