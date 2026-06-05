// This sets layers based on a Windows new layer 2D when it is restacked in canvas
zox_sys2(WindowLayerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SetWindowLayer);
    zox_sys_in(CanvasLink);
    zox_sys_out(WindowLayer);
    zox_sys_out(Layer2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SetWindowLayer, nlayer);
        zox_sys_i(CanvasLink, canvasLink);
        zox_sys_o(WindowLayer, wlayer);
        zox_sys_o(Layer2D, layer2D);
        if (wlayer->value == nlayer->value) {
            continue;
        }
        if (!zox_valid(canvasLink->value) || !zox_has(canvasLink->value, WindowsLayers)) {
            continue;
        }
        zox_geter_value(canvasLink->value, WindowsLayers, byte, layers_per_window);
        wlayer->value = nlayer->value;
        byte window_layer = wlayer->value;
        layer2D->value = window_layer * layers_per_window;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_element_layers_auto(world, e2, layer2D->value);
                set_element_layers(world, e2, layer2D->value);
            }
        }
    }
} zox_sys_end(WindowLayerSystem);
