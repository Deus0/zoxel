entity spawn_realm_quest(
    ecs* world,
    entity prefab,
    entity parent,
    const char* name,
    const char* tname)
{
    // prefab_child instead?
    entity e = zox_ins_named(world, prefab);
    zox_make_prefab(e);
    if (name) {
        zox_set_unique_name(e, name);
        set_ZoxName(world, e, name);
    }
    entity texture = string_hashmap_get(
        files_hashmap_textures,
        new_string_data(tname));
    if (zox_valid(texture)) {
        zox_link(world, e, TextureLink, texture);
    }
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    return e;
}
