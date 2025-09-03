// spawn prefabs for each meta stats
entity spawn_stat_state_t(ecs* world,
    const char* name,
    char* texture_name,
    const color_rgb c)
{
    const entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    entity e = create_stat_state(world, name, texture, c);
    return e;
}

entity spawn_stat_level_t(ecs* world,
    const char* name,
    char* texture_name,
    const color_rgb c)
{
    const entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    entity e = create_stat_level(world, name, texture, c);
    return e;
}

entity spawn_stat_regen_t(ecs* world,
    const char* name,
    char* texture_name)
{
    const entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    entity e = create_stat_regen(world, name, texture);
    return e;
}

entity spawn_stat_attribute_t(ecs* world,
    const char* name,
    char* texture_name)
{
    const entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    entity e = create_stat_attribute(world, name, texture);
    return e;
}
