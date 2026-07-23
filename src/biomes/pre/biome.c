entity spawn_prefab_biome(ecs* world) {
    zox_prefab();
    zox_prefab_name("biome");
    zox_add_tag(e, Biome);
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, Seed, { 0 });
    zox_prefab_set(e, RealmLink, { 0 });
    zox_prefab_set(e, BiomeSkyColor, { color_rgb_cyan });
    zox_prefab_set(e, DirtChance, { 0 });
    zox_prefab_set(e, GrassChance, { 1 });
    zox_prefab_set(e, WeedsChance, { 1 });
    zox_prefab_set(e, TreeChance, { 1 });
    zox_prefab_set(e, FlowerChance, { 1 });
    zox_prefab_set(e, BiomeHeightFrequency, { 0 });
    // Events
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    return e;
}

    // zox_prefab_set(e, Colors, { 0 });
    // zox_prefab_set(e, ModelLinks, { 0 });
    // zox_prefab_set(e, BlockLinks, { 0 });