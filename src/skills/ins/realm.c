entity spawn_realm_skill(ecs* world, entity parent, entity prefab, const char* name, const char* tname, color ecolor) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(tname));
    if (!zox_valid(texture)) {
        zox_loge("Stat Texture not found [%s]", tname);
    }
    entity e = zox_ins_named(world, prefab);
    zox_make_prefab(e);
    if (name) {
        zox_set_unique_name(e, name);
        set_ZoxName(world, e, name);
    }
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    if (texture) {
        zox_set(e, TextureLink, { texture });
    }
    if (!color_equals(ecolor, color_white)) {
        zox_set(e, Color, { ecolor });
    }
    return e;
}
