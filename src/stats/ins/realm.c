entity spawn_realm_stat(
    ecs* world,
    entity parent,
    entity prefab,
    const char* name,
    char* texture_name,
    color_rgb ecolor)
{
    entity texture = string_hashmap_get(
        files_hashmap_textures,
        new_string_data(texture_name));
    if (!zox_valid(texture)) {
        zox_loge("Stat Texture not found [%s]", texture_name);
    }
    entity e = zox_prefab_from_parent(world, prefab);
    // entity e = zox_ins_named(world, prefab);
    // zox_make_prefab(e);
    // zox_add(e, RealmStat);
    if (name) {
        zox_set_unique_name(e, name);
        set_ZoxName(world, e, name);
    }
    if (texture) {
        zox_link(world, e, TextureLink, texture);
    }
    if (!color_rgb_equals(ecolor, color_rgb_white)) {
        zox_setv(e, ColorRGB, ecolor);
    }
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    return e;
}

entity spawn_realm_stat_regen(
    ecs* world,
    entity parent,
    entity prefab,
    const char* name,
    char* texture_name,
    color_rgb ecolor,
    entity target_stat)
{
    entity e = spawn_realm_stat(
        world,
        parent,
        prefab,
        name,
        texture_name,
        ecolor);
    zox_link(world, e, StatLink, target_stat);
    return e;
}
