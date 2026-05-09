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
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        set_element_layers_auto(world, child, new_layer);
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
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        set_element_layers(world, child, window_layer);
    }
}


// todo: implement localLayer2D's here for elements'
byte get_highest_layer(ecs *world, entity e, byte layer) {
    if (!e) {
        return layer;
    }
    byte child_layer = layer + 1;
    byte highest_layer = layer;
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity child = children[j];
        byte new_layer = get_highest_layer(world, child, child_layer);
        // if (new_layer > highest_layer) zox_log("    > [%lu] layers_per_window at %i / %i [%i]\n", e, j, children->length, new_layer)
        if (new_layer > highest_layer) {
            highest_layer = new_layer;
        }
    }
    return highest_layer;
}
