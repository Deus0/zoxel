// NOTE: These are our realm stat helpers for spawning
entity spawn_stat_state_t(ecs* world, const char* name, char* texture_name, color_rgb stat_color) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    //entity e = create_stat_state(world, name, texture, c);
    //return e;
    return spawn_stat(world, prefab_stat_state, 0, name, texture, stat_color);
}

entity spawn_stat_level_t(ecs* world, const char* name, char* texture_name, color_rgb stat_color) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    //entity e = create_stat_level(world, name, texture, c);
    //return e;
    return spawn_stat(world, prefab_stat_level, 0, name, texture, stat_color);
    // return spawn_stat_level(world, prefab_stat_level, 0, 0, name, texture);
}

entity spawn_stat_regen_t(ecs* world, const char* name, char* texture_name) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    //entity e = create_stat_regen(world, name, texture);
    //return e;
    return spawn_stat(world, prefab_stat_regen, 0, name, texture, color_rgb_white);
    // return spawn_stat_regen(world, prefab_stat_regen, 0, 0, name, texture);
}

entity spawn_stat_attribute_t(ecs* world, const char* name, char* texture_name) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    // ecs* world, entity prefab, entity parent, float value, entity texture
    // return spawn_stat_attribute(world, prefab_stat_attribute, 0, 0, name, texture);
    //entity e = create_stat_attribute(world, name, texture);
    //return e;
    return spawn_stat(world, prefab_stat_attribute, 0, name, texture, color_rgb_white);
}
