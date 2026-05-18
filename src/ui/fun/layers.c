void set_element_layers_auto(ecs *world, entity e, byte layer) {
    if (!zox_valid(e)) {
        return;
    }
    byte new_layer = layer + 1;
    if (zox_has(e, BonusLayer2)) {
        new_layer += zox_gett_value(e, BonusLayer2);
    }
    if (zox_has(e, Layer2D)) {
        zox_set(e, Layer2D, { new_layer })
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_element_layers_auto(world, e2, new_layer);
        }
    }
}

void set_element_layers(ecs *world, entity e, byte window_layer) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, ElementLayer) && zox_has(e, Layer2D)) {
        zox_geter_value(e, ElementLayer, byte, element_layer);
        byte new_layer = window_layer + element_layer;
        if (zox_has(e, BonusLayer2)) {
            new_layer += zox_gett_value(e, BonusLayer2);
        }
        zox_set(e, Layer2D, { new_layer });
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            set_element_layers(world, e2, window_layer);
        }
    }
}

// todo: implement localLayer2D's here for elements'
byte get_highest_layer(ecs *world, entity e, byte layer) {
    if (!e) {
        return layer;
    }
    byte child_layer = layer + 1;
    byte highest_layer = layer;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            byte new_layer = get_highest_layer(world, e2, child_layer);
            // if (new_layer > highest_layer) zox_log("    > [%lu] layers_per_window at %i / %i [%i]\n", e, j, children->length, new_layer)
            if (new_layer > highest_layer) {
                highest_layer = new_layer;
            }
        }
    }
    return highest_layer;
}
