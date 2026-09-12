entity spawn_biome(
    ecs *world,
    const entity prefab,
    const entity realm,
    char* name,
    lint seed
) {
    zox_instance(prefab);
    zox_link(world, e, RealmLink, realm);
    zox_name(name);
    set_ZoxName(world, e, name);
    zox_setv(e, Seed, seed);
    return e;
}