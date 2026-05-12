// NOTE: These are our realm stat helpers for spawning
/*entity spawn_stat_state_t(ecs* world, const char* name, char* texture_name, color_rgb stat_color) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    return spawn_stat(world, prefab_stat_state, 0, name, texture, stat_color);
}

entity spawn_stat_level_t(ecs* world, const char* name, char* texture_name, color_rgb stat_color) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    return spawn_stat(world, prefab_stat_level, 0, name, texture, stat_color);
}

entity spawn_stat_regen_t(ecs* world, const char* name, char* texture_name) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    return spawn_stat(world, prefab_stat_regen, 0, name, texture, color_rgb_white);
}

entity spawn_stat_attribute_t(ecs* world, const char* name, char* texture_name) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    return spawn_stat(world, prefab_stat_attribute, 0, name, texture, color_rgb_white);
}
// */