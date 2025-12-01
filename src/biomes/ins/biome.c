entity spawn_biome(
    ecs *world,
    const entity prefab,
    char* name
) {
    zox_instance(prefab);
    zox_name(name);
    set_ZoxName(world, e, name);
    return e;
}