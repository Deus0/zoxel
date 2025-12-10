entity spawn_prefab_biome(ecs* world) {
    zox_prefab();
    zox_prefab_name("biome");
    zox_add_tag(e, Biome);
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, Colors, { 0 });
    zox_prefab_set(e, ModelLinks, { 0 });
    zox_prefab_set(e, BlockLinks, { 0 });
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    return e;
}