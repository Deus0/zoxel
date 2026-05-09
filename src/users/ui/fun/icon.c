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
