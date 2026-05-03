zox_sys2(WindowLayerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SetWindowLayer);
    zox_sys_in(CanvasLink);
    zox_sys_in(Children);
    zox_sys_out(WindowLayer);
    zox_sys_out(Layer2D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SetWindowLayer, nlayer);
        zox_sys_i(CanvasLink, canvasLink);
        zox_sys_i(Children, children);
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

        for (int j = 0; j < children->length; j++) {
            entity child = children->value[j];
            set_element_layers_auto(world, child, layer2D->value);
            set_element_layers(world, child, layer2D->value);
        }
    }
} zox_sys_end(WindowLayerSystem);
