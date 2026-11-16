entity spawn_block_texture(
    ecs *world,
    byte index,
    char *name,
    char *texture_filename)
{
    SpawnBlock spawn_data = {
        .prefab = prefab_block,
        .index = index,
        .seed = generate_voxel_seed(index),
        .name = name,
        .color = color_black,
        .textures = 1,
        .prefab_texture = prefab_vox_texture,
        .texture_filename = texture_filename,
    };
    entity e = spawn_block(
        world,
        spawn_data);
    zox_add(e, BlockBaked);
    zox_add(e, SingleTextureBlock);
    return e;
}

// a realm block drawn with one generated texture on every face, i.e. an animated one (prefab_texture_water)
entity spawn_realm_block_textured(
    ecs *world,
    entity parent,
    entity prefab_texture,
    lint seed,
    const char* name,
    color primary)
{
    entity e = zox_ins(world, prefab_block);
    set_ZoxName(world, e, name);
    zox_set_unique_name(e, name);
    zox_setv(e, Seed, seed);
    zox_setv(e, Color, primary);
    zox_add(e, BlockBaked);
    zox_add(e, SingleTextureBlock);
    zox_set_parent(world, e, parent);
    entity texture = zox_ins(world, prefab_texture);
    zox_set_unique_name(texture, name);
    // the size of every block's texture now: the Block Res setting resizes the block texture prefabs, not this one's
    //  (a 16 pixel water texture in 32 pixel tiles showed at a quarter of its face)
    zox_setv(texture, TextureSize, int2_single(powers_of_two[block_depth]));
    zox_setv(texture, Seed, seed);
    zox_setv(texture, FillColor, primary);
    zox_setv(texture, ChildIndex, 0);
    zox_link(world, texture, BlockLink, e);
    zox_set_parent(world, texture, e);
    return e;
}
