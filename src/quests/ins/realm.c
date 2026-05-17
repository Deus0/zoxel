entity spawn_realm_quest(ecs* world, entity parent, entity prefab, const char* name, const char* tname) {
    entity e = zox_ins_named(world, prefab);
    zox_make_prefab(e);
    if (name) {
        zox_set_unique_name(e, name);
        set_ZoxName(world, e, name);
    }
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(tname));
    if (zox_valid(texture)) {
        zox_set(e, TextureLink, { texture });
    }
    return e;
}
