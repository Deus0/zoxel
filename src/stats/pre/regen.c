entity create_stat_regen(ecs *world,
    const char *name,
    const entity texture)
{
    entity e = spawn_meta_stat(world, prefab_stat, name);
    zox_add_tag(e, StatRegen)
    zox_prefab_set(e, StatValue, { 10 })
    zox_prefab_set(e, TextureLink, { texture })
    return e;
}