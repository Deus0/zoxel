entity create_stat_attribute(
    ecs *world,
    const char *name,
    const entity texture
) {
    entity e = spawn_meta_stat(world, prefab_stat, name);
    zox_add_tag(e, StatAttribute);
    zox_prefab_set(e, StatValue, { 0 });
    // zox_prefab_set(e, StatValueMax, { 10 })
    zox_prefab_set(e, TextureLink, { texture });
    return e;
}