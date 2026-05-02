/*void set_icon_label_from_user_data_quantity(
    ecs *world,
    const entity e,
    const byte quantity
) {
    if (!zox_has(e, LabelPrefabLink)) {
        return;
    }

    if (!zox_has(e, Children)) {
        zox_log_error("Frame has no children :( %lu", e);
        return;
    }

    zox_geter(e, Children, children);
    if (children->length < 2) {
        return;
    }

    const entity zext = children->value[1];
    if (!zox_valid(zext)) {
        return;
    }

    if (quantity > 1) {
        char text[6];
        sprintf(text, "x%i", quantity);
        set_entity_text(world, zext, text);
    } else {
        set_entity_text(world, zext, "");
        // set_entity_text(world, zext, "testt");
    }
}

void set_icon_label_from_user_data(
    ecs *world,
    const entity e,
    const entity data
) {
    if (!zox_valid(e) || !zox_has(e, LabelPrefabLink)) {
        return;
    }
    if (!zox_has(e, Children)) {
        zox_log_error("Frame has no children :( %lu", e);
        return;
    }

    zox_geter(e, Children, children);
    if (children->length < 2) {
        return;
    }

    const entity zext = children->value[1];
    if (!zox_valid(zext)) {
        return;
    }

    const byte quantity = zox_valid(data) && zox_has(data, Quantity) ? zox_gett_value(data, Quantity) : 0;
    // zox_log("x%i\n", quantity)
    if (quantity > 1) {
        char text[6];
        sprintf(text, "x%i", quantity);
        set_entity_text(world, zext, text);
    } else {
        set_entity_text(world, zext, "");
        // set_entity_text(world, zext, "test");
    }
}

void set_icon_label_from_user_data_direct(
    ecs *world,
    const entity e, // label
    const entity data
) {
    if (!zox_valid(e) || !zox_valid(data)) {
        return;
    }
    if (zox_has(data, Quantity)) {
        zox_geter_value(data, Quantity, byte, quantity);
        if (quantity > 1) {
            char text[6];
            sprintf(text, "x%i", quantity);
            set_entity_text_raw(world, e, text);
        }
    }
}*/

void set_icon_from_user_data(ecs *world, entity frame, entity icon, entity data) {

    if (!zox_valid(frame) || !zox_valid(icon)) {
        zox_log_error("invalid frame or e in icon setting")
        return;
    }

    zox_set(frame, UserDataLink, { data });
    zox_set(icon, UserDataLink, { data });
    entity texture = zox_valid(data) && zox_has(data, TextureLink) ? zox_gett_value(data, TextureLink) : 0;
    if (!texture) {
        if (zox_valid(data)) {
            texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
        } else {
            texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
        }
    }

    clone_texture_data(world, icon, texture);

    // zox_log("set usericon data %s %s", zox_get_name(e), zox_get_name(texture));
}
