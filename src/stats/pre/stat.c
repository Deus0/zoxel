entity spawn_prefab_stat(ecs* world) {
    zox_prefab();
    zox_prefab_name("stat");
    zox_prefab_set(e, ZoxName, { 0 });
    zox_add(e, Stat);
    zox_prefab_set(e, StatValue, { 0 });
    zox_prefab_set(e, StatDirty, { zox_dirty_none });
    zox_prefab_set(e, ColorRGB, { color_rgb_white });
    // zox_prefab_set(e, TextureLink, { 0 });
    return e;
}
