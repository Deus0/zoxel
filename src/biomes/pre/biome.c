entity spawn_prefab_biome(ecs* world) {
    zox_prefab();
    zox_prefab_name("biome");
    zox_add_tag(e, Biome);
    zox_set(e, Colors, { 0 });
    return e;
}