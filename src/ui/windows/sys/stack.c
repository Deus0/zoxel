// #define zox_debug_canvas_stack
void set_recursively_layer2(ecs* world, entity e, byte value) {
    if (!zox_has(e, Layer2D)) {
        return;
    }
    zox_muter(e, Layer2D, layer2);
    layer2->value = value;
    value++;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2)) {
                continue;
            }
            set_recursively_layer2(world, e2, value);
        }
    }
}

byte get_highest_window_layer(ecs* world, entity e) {
    byte layer = 0;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2) || !zox_has(e2, Window) || zox_has(e2, IgnoreWindowLayering)) {
                continue;
            }
            byte window_layers = get_highest_layer(world, e2, 1);
            if (window_layers > layer) {
                layer = window_layers;
            }
        }
    }
    return layer;
}

/*yte2 count_windows_in_stack(ecs *world, const entity* children, uint children_length) {
    byte windows_count = 0;
    byte layers_per_window = 1;
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        if (!zox_valid(child) || !zox_has(child, Window) || zox_has(child, IgnoreWindowLayering)) {
            continue;
        }
        byte window_layers = get_highest_layer(world, child, 1);
        if (window_layers > layers_per_window) {
            layers_per_window = window_layers;
        }
        windows_count++;
    }
    return (byte2) { windows_count, layers_per_window };
}*/

// Reorders windows on a stack, moves all previous ones up the stack when a new one is placed
zox_sys2(CanvasStackSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(WindowToTop);
    zox_sys_out(WindowsLayers);
    zox_sys_out(WindowsCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(WindowToTop, add_window);
        zox_sys_o(WindowsLayers, windowsLayers);
        zox_sys_o(WindowsCount, windowsCount);
        if (!zox_valid(add_window->value) || !zox_has(add_window->value, WindowLayer) || zox_has(add_window->value, IgnoreWindowLayering)) {
            continue;
        }
        byte old_layer = zox_get_value(add_window->value, WindowLayer);
        // entity childrens[layouts2_children_capacity];
        // uint children_length = zox_get_children(world, e, childrens, layouts2_children_capacity);
        // byte2 counter = count_windows_in_stack(world, childrens, children_length);
        uint windows_count = zox_get_children_count(world, e);
        // byte windows_count = counter.x; // maybe count windows first
        // byte layers_per_window = counter.y;
        byte layers_per_window = get_highest_window_layer(world, e);
        // gett previous window layer of moving to top window
        // skip if: same window clicked as already on top
        if (windows_count == windowsCount->value && old_layer == windows_count) {
            add_window->value = 0;
            #ifdef zox_debug_canvas_stack
            zox_log(" > skipping stack refresh as same window as last selected\n", old_layer)
            #endif
            continue;
        }
        // set_recursively_layer2(world, add->window, windows_count * layers_per_window);
        zox_set(add_window->value, SetWindowLayer, { windows_count });
        windowsLayers->value = layers_per_window;
        byte old_windows_count = windowsCount->value;
        byte not_assigned_index = windowsCount->value + 1; // start on top of stack, but below latest
        int max_checks = 255;
        windowsCount->value = windows_count;
        int_hashmap *windows = create_int_hashmap(windows_count);
        int_hashmap_add(windows, windows_count, add_window->value);
        #ifdef zox_debug_canvas_stack
        zox_log(" >  canvas stack system refreshing [%i] out of [%i]\n", windows_count, children->length)
        zox_log("   - [%i] event WindowLayer [%lu]\n", windows_count, add_window->value)
        #endif

        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
        // for (uint j = 0; j < children_length; j++) {
        //    entity child = childrens[j];
                if (!zox_valid(e2) || !zox_has(e2, Window) || zox_has(e2, IgnoreWindowLayering)) {
                    continue;
                }
                if (add_window->value == e2) {
                    continue;
                }
                byte child_window_layer = zox_get_value(e2, WindowLayer);
                // if on top
                if (child_window_layer == 0) {
                    // if (int_hashmap_has(windows, not_assigned_index)) zox_log("   ! issue with index, possible removal\n")
                    byte new_layer = not_assigned_index;
                    int checks = 0;
                    while (int_hashmap_has(windows, new_layer)) {
                        if (new_layer == 1) {
                            new_layer = old_windows_count;
                        } else {
                            new_layer--;
                        }
                        #ifdef zox_debug_canvas_stack
                        zox_log("   ! issue with index, lowering [%i]\n", new_layer)
                        #endif
                        checks++;
                        if (checks > max_checks) {
                            zox_log("FAILED CANVAS STACKING 1\n")
                            int_hashmap_dispose(windows);
                            add_window->value = 0;
                            return; // failure!
                        }
                    }
                    // int_hashmap_add(windows, new_layer, add_window->value);
                    if (!int_hashmap_has(windows, new_layer)) {
                        int_hashmap_add(windows, new_layer, e2);
                    } else {
                        zox_log("! layer already stacked [%i]\n", new_layer)
                    }
                    zox_set(e2, SetWindowLayer, { new_layer })
                    not_assigned_index++;
                    #ifdef zox_debug_canvas_stack
                    zox_log("   - [%i] setting new child WindowLayer [%lu]\n", new_layer, e2)
                    #endif
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
                        #ifdef zox_debug_canvas_stack
                        zox_log("   ! issue with index, lowering [%i]\n", new_layer)
                        #endif
                        checks++;
                        if (checks > max_checks) {
                            zox_log("FAILED CANVAS STACKING 2\n")
                            int_hashmap_dispose(windows);
                            add_window->value = 0;
                            return; // failure!
                        }
                    }
                    if (!int_hashmap_has(windows, new_layer)) {
                        int_hashmap_add(windows, new_layer, e2);
                    } else {
                        zox_log("! layer already stacked [%i]\n", new_layer)
                    }
                    zox_set(e2, SetWindowLayer, { new_layer });
                    #ifdef zox_debug_canvas_stack
                    zox_log("   - [%i] decreasing WindowLayer [%lu]\n", new_layer, e2)
                    #endif
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
                            #ifdef zox_debug_canvas_stack
                            zox_log("   ! issue with index, lowering [%i]\n", new_layer)
                            #endif
                            checks++;
                            if (checks > max_checks) {
                                zox_log("FAILED CANVAS STACKING 3\n")
                                int_hashmap_dispose(windows);
                                add_window->value = 0;
                                return; // failure!
                            }
                        }
                        if (!int_hashmap_has(windows, new_layer)) {
                            int_hashmap_add(windows, new_layer, e2);
                        } else {
                            zox_log("! layer already stacked [%i]\n", new_layer)
                        }
                        zox_set(e2, SetWindowLayer, { new_layer })
                        #ifdef zox_debug_canvas_stack
                        zox_log("   - [%i] anew WindowLayer [%lu]\n", new_layer, e2)
                        #endif
                    } else {
                        int_hashmap_add(windows, child_window_layer, e2);
                        #ifdef zox_debug_canvas_stack
                        zox_log("   - [%i] keeping WindowLayer [%lu]\n", child_window_layer, e2)
                        #endif
                    }
                }
            }
        }
        int_hashmap_dispose(windows);
        add_window->value = 0;
    }
} zox_sys_end(CanvasStackSystem);
