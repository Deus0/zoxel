entity spawn_realm_stat(ecs* world, entity parent, entity prefab, const char* name, char* texture_name, color_rgb ecolor) {
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    if (!zox_valid(texture)) {
        zox_loge("Stat Texture not found [%s]", texture_name);
    }
    entity e = zox_ins_named(world, prefab);
    zox_make_prefab(e);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    zox_add(e, RealmStat);
    if (name) {
        zox_set_unique_name(e, name);
        set_ZoxName(world, e, name);
    }
    if (texture) {
        zox_set(e, TextureLink, { texture });
    }
    if (!color_rgb_equals(ecolor, color_rgb_white)) {
        zox_set(e, ColorRGB, { ecolor });
    }
    return e;
}
