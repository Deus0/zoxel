// our main realm block spawn function
entity spawn_block_vox_meta(
    ecs *world,
    SpawnBlock data)
{
    byte depth = block_depth;
    entity e = zox_ins(world, data.prefab);
    set_ZoxName(world, e, data.name);
    zox_set_unique_name(e, data.name);
    zox_setv(e, BlockIndex, data.index);
    zox_setv(e, Color, data.color);
    if (data.prefab_world_block) {
        zox_prefab_set(e, BlockPrefabLink, { data.prefab_world_block });
    }
    if (data.tag) {
        zox_add_id(e, data.tag);
    }
    if (data.disable_collision) {
        zox_set(e, BlockCollider, { zox_block_air });
    }
    if (data.model_type) {
        zox_set(e, BlockModel, { data.model_type });
    }
    if (data.model) {
        zox_set(e, ModelLink, { data.model });
    }
    if (data.vox_offset) {
        zox_set(e, BlockVoxOffset, { 1 });
    }
    // if not bake
    if (!data.bake_vox || !zox_valid(data.prefab_texture)) {
        return e;
    }
    int2 vox_texture_size = int2_single(powers_of_two[depth]);
    // TextureLinks textures = (TextureLinks) { 0 };
    // initialize_TextureLinks(&textures, 6);
    for (byte i = 0; i < 6; i++) {
        entity e2 = spawn_texture(
            world,
            data.prefab_texture,
            vox_texture_size);
        zox_set_name_e(e2, "texture_block");
        zox_set(e2, VoxBakeSide, { i });
        zox_set(e2, ModelLink, { data.model });
        zox_link(world, e2, BlockLink, e);
        zox_set_parent(world, e2, e);
        // textures.value[i] = e2;
    }
    // zox_set_ptr(e, TextureLinks, textures);
    // zox_log("Block [%s]: Vox Texture Size [%ix%i] depth [%i]", data.name, vox_texture_size.x, vox_texture_size.y, depth);
    // zox_log(" + generated block [vox] name [%s]\n", name)
    return e;
}
