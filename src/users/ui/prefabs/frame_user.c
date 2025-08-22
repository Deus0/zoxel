entity spawn_prefab_frame_user(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab)
    zox_prefab_name("frame_user")
    zox_prefab_set(e, UserDataLink, { 0 })
    zox_prefab_add(e, TooltipEvent)
    return e;
}

void set_icon_label_from_user_data_quantity(
    ecs *world,
    const entity frame,
    const byte quantity
) {
    if (!zox_has(frame, IconLabel)) {
        return;
    }
    if (!zox_has(frame, Children)) {
        zox_log(" ! frame has no children :( %lu\n", frame)
    } else {
        const Children *children = zox_get(frame, Children)
        if (children->length < 2) {
            return;
        }
        const entity zext = children->value[1];
        if (quantity > 1) {
            char text[6];
            sprintf(text, "x%i", quantity);
            set_entity_text(world, zext, text);
        } else {
            set_entity_text(world, zext, "");
        }
    }
}

void set_icon_label_from_user_data(
    ecs *world,
    const entity frame,
    const entity data
) {
    if (!zox_valid(frame) || !zox_has(frame, IconLabel)) {
        return;
    }
    if (!zox_has(frame, Children)) {
        zox_log(" ! frame has no children :( %lu\n", frame)
        return;
    }
    zox_geter(frame, Children, children);
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
    }
}

void set_icon_label_from_user_data_direct(
    ecs *world,
    const entity zext,
    const entity data
) {
    if (!zox_valid(zext) || !zox_valid(data)) {
        return;
    }
    if (zox_has(data, Quantity)) {
        const byte quantity = zox_get_value(data, Quantity)
        // zox_log("x%i\n", quantity)
        if (quantity > 1) {
            char text[6];
            sprintf(text, "x%i", quantity);
            set_entity_text_raw(world, zext, text);
        } else {
        //    set_entity_text(world, zext, "");
        }
    } else {
    //    set_entity_text(world, zext, "");
    }
}

void set_icon_from_user_data(
    ecs *world,
    const entity frame,
    const entity e,
    const entity data
) {
    if (!zox_valid(frame) || !zox_valid(e)) {
        zox_log_error("invalid frame or e in icon setting")
        return;
    }
    zox_set(frame, UserDataLink, { data });
    zox_set(e, UserDataLink, { data });
    /*if (!zox_valid(data)) {
        /*clear_texture_data(world, e);
        zox_set(e, GenerateTexture, { zox_generate_texture_trigger });
        zox_set(e, TextureSize, { int2_single(default_icon_texture_size) });
        return;
    }*/
    /*if (!zox_has(data, TextureLink)) {
        zox_log_error("[%s] has no texture link component: %i", zox_get_name(data), zox_has(data, TextureLink))
        return;
    }*/
    entity texture = zox_valid(data) && zox_has(data, TextureLink) ? zox_gett_value(data, TextureLink) : 0;
    if (!texture) {
        if (zox_valid(data)) {
            texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
        } else {

        }
        /*clear_texture_data(world, e);
        zox_set(e, GenerateTexture, { zox_generate_texture_trigger })
        zox_set(e, TextureSize, { int2_single(default_icon_texture_size) })
        zox_log_error("[%s] has no texture", zox_get_name(data))
        return;*/
    }
    clone_texture_data(world, e, texture);

    // zox_log("set usericon data %s %s", zox_get_name(e), zox_get_name(texture));
}

entity spawn_frame_user(
    ecs *world,
    SpawnFrame data,
    const entity userdata
) {
    const entity3 e = spawn_frame(world, data);
    set_icon_from_user_data(world, e.x, e.y, userdata);
    set_icon_label_from_user_data_direct(world, e.z, userdata);
    zox_set_unique_name(e.x, "frame_user");
    zox_set_unique_name(e.y, "icon_user");
    zox_set_unique_name(e.z, "icon_label_user");
    return e.x;
}