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
