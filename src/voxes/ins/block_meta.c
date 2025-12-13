// our main realm block spawn function
entity spawn_block_vox_meta(ecs *world, const SpawnBlock data) {
    zox_instance(data.prefab);
    zox_name(data.name);
    set_ZoxName(world, e, data.name);
    zox_set(e, BlockIndex, { data.index });
    zox_set(e, Color, { data.color });
    if (data.prefab_world_block) {
        zox_prefab_set(e, BlockPrefabLink, { data.prefab_world_block });
    }
    if (data.tag) {
        zox_add_tag_id(e, data.tag);
    }
    if (data.disable_collision) {
        zox_set(e, BlockCollider, { zox_block_air });
    }
    if (data.model) {
        zox_set(e, BlockModel, { data.model });
    }
    if (data.vox) {
        zox_set(e, ModelLink, { data.vox });
    }
    if (data.vox_offset) {
        zox_set(e, BlockVoxOffset, { 1 });
    }
    if (data.bake_vox) {
        TextureLinks textures = (TextureLinks) { 0 };
        initialize_TextureLinks(&textures, 6);
        for (byte i = 0; i < 6; i++) {
            const entity e2 = spawn_texture(
                world,
                data.prefab_texture,
                voxel_texture_size
            );
            zox_set_name_e(e2, "texture_block");
            zox_set(e2, VoxLink, { data.vox });
            zox_set(e2, VoxBakeSide, { i });
            textures.value[i] = e2;
        }
        zox_set_ptr(e, TextureLinks, textures);
    }
    // zox_log(" + generated block [vox] name [%s]\n", name)
    return e;
}