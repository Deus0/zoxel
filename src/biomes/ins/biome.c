entity spawn_biome(
    ecs *world,
    const entity prefab,
    const entity realm,
    char* name,
    lint seed
) {
    zox_instance(prefab);
    zox_set(e, RealmLink, { realm });
    zox_name(name);
    set_ZoxName(world, e, name);
    zox_set(e, Seed, { seed });
    return e;
}