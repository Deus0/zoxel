// a block that is for a block vox
entity spawn_prefab_block_base(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("block_base");
    zox_prefab_add(e, ModelLink);
    return e;
}


entity spawn_prefab_block_baked(
    ecs* world,
    entity parent)
{
    zox_instance(parent);
    zox_name("block_baked");
    zox_add(e, BlockBaked);
    return e;
}
