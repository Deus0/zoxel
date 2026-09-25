entity spawn_prefab_stat(ecs* world) {
    zox_prefab();
    zox_prefab_name("stat");
    zox_set(e, ZoxName, { 0 });
    zox_add(e, Stat);
    zox_setv(e, StatValue, 0);
    zox_setv(e, ColorRGB, color_rgb_white);
    return e;
}
