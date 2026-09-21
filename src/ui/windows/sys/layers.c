void set_layers_auto(
    ecs *world,
    entity e,
    byte layer,
    byte dbg_log)
{
    if (!zox_valid(e)) {
        return;
    }
    byte new_layer = layer + 1;
    if (zox_has(e, BonusLayer)) {
        new_layer += zox_getv(e, BonusLayer);
    }
    if (zox_has(e, Layer)) {
        zox_setv(e, Layer, new_layer);
        if (dbg_log) {
            zox_log(" - [%s]: %i",
                zox_getn(e),
                new_layer);
        }
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_layers_auto(
                world,
                e2,
                new_layer,
                dbg_log);
        }
    }
}

// This sets layers based on a Windows new layer 2D when it is restacked in canvas
// TODO: We can just set window layer when stacking?
zox_sys2(WindowLayerSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WindowLayer);
    zox_sys_out(Layer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WindowLayer, window_layer);
        zox_sys_o(Layer, layer);
        if (window_layer->value) {
            entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
            if (!zox_valid(canvas) || !zox_has(canvas, WindowsLayers)) {
                continue;
            }
            byte layers_per_window = zox_getv(canvas, WindowsLayers);
            layer->value = window_layer->value * layers_per_window;
        } else {
            layer->value = 0;
        }
        if (zox_has(e, BonusLayer)) {
            layer->value += zox_getv(e, BonusLayer);
        }
        if (dbg_log) {
            zox_log("Window [%s] Layers Set [%i]", zox_sys_e_name, layer->value);
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_layers_auto(
                    world,
                    e2,
                    layer->value,
                    dbg_log);
            }
        }
        zox_remove(e, WindowLayerDirty);
    }
} zox_sys_end(WindowLayerSystem);
