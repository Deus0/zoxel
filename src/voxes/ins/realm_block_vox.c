// our main realm block spawn function
/*entity spawn_realm_block_vox(ecs *world, entity prefab, entity parent, lint seed, const char* name, color primary, byte is_collision, entity model, byte model_type, byte is_bake) {
    byte depth = block_vox_depth;
    entity e = zox_ins(prefab);
    zox_set_parent(world, e, parent);
    set_ZoxName(world, e, name);
    zox_set_unique_name(e, name);
    zox_set(e, Seed, { seed });
    zox_set(e, Color, { primary });
    zox_set(e, BlockCollider, { is_collision ? zox_block_solid : zox_block_air });
    zox_set(e, BlockModel, { model_type });
    if (model) {
        // TODO: Juset set model type if model
        zox_set(e, ModelLink, { model });
    }
    // if not bake
    // TODO: Just spawn these onto prefab of BlockVoxBaked
    if (is_bake) {
        entity prefab_texture = prefab_vox_texture;
        int2 vox_texture_size = int2_single(powers_of_two[depth]);
        TextureLinks textures = (TextureLinks) { 0 };
        initialize_TextureLinks(&textures, 6);
        for (byte i = 0; i < 6; i++) {
            entity e2 = spawn_texture(world, prefab_texture, vox_texture_size);
            zox_set_name_e(e2, "texture_block");
            zox_set(e2, VoxBakeSide, { i });
            zox_set(e2, ModelLink, { model });
            zox_set(e2, BlockLink, { e });
            textures.value[i] = e2;
        }
        zox_set_ptr(e, TextureLinks, textures);
    }
    return e;
}
*/
