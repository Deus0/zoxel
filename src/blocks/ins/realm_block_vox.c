// our main realm block spawn function
entity spawn_realm_block_solid(
    ecs *world,
    entity prefab,
    entity parent,
    lint seed,
    const char* name,
    color primary,
    entity model,
    byte dbg_log)
{
    byte depth = block_depth;
    entity e = zox_ins(world, prefab);
    set_ZoxName(world, e, name);
    zox_set_unique_name(e, name);
    zox_setv(e, Seed, seed);
    zox_setv(e, Color, primary);
    zox_setv(e, BlockCollider, zox_block_solid);
    zox_setv(e, BlockModel, zox_block_solid);
    zox_set_parent(world, e, parent);
    if (model) {
        // TODO: Juset set model type if model
        zox_setv(e, ModelLink, model);
    }
    // if not bake
    // TODO: Just spawn these onto prefab of BlockVoxBaked
    int2 vox_texture_size = int2_single(powers_of_two[depth]);
    //TextureLinks textures = (TextureLinks) { 0 };
    //initialize_TextureLinks(&textures, 6);
    for (byte i = 0; i < 6; i++) {
        entity e2 = spawn_texture(
            world,
            prefab_vox_texture,
            vox_texture_size);
        {
            char name2[64];
            sprintf(name2, "texture_%s", name);
            zox_set_unique_name(e2, name2);
            if (dbg_log) {
                zox_log("Spawned Vox Texture [%s]:%i", name2, i);
            }
        }
        zox_setv(e2, VoxBakeSide, i);
        zox_setv(e2, ModelLink, model);
        zox_link(world, e2, BlockLink, e);
        zox_setv(e2, ChildIndex, i);
        zox_set_parent(world, e2, e);
        // textures.value[i] = e2;
    }
    // zox_set_ptr(e, TextureLinks, textures);
    return e;
}
