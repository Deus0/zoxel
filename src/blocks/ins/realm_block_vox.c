// our main realm block spawn function
// a solid block, its 6 faces baked from its model
//  - bake_on_done: its faces bake once the model's done (a blueprint: vox_texture_system waits on Generating), else
//    when its generated model says (GenerateModel's bake step, bake_vox_system)
entity spawn_realm_block_baked_model(
    ecs *world,
    entity prefab,
    entity parent,
    lint seed,
    const char* name,
    color primary,
    entity model,
    byte bake_on_done,
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
    if (model) {
        zox_set_link(world, e, ModelLink, model);
    }
    zox_set_parent(world, e, parent);
    // if not bake
    // TODO: Just spawn these onto prefab of BlockVoxBaked
    byte length = octree_size(depth);
    int2 vox_texture_size = int2_single(length);
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
        zox_setv(e2, ChildIndex, i);
        zox_setv(e2, VoxBakeSide, i);
        zox_set_link(world, e2, ModelLink, model);
        zox_link(world, e2, BlockLink, e);
        zox_set_parent(world, e2, e);
        if (bake_on_done) {
            zox_setv(e2, GenerateTexture, zox_generate_texture_run);
        }
    }
    return e;
}

entity spawn_realm_block_solid(ecs *world, entity prefab, entity parent, lint seed, const char* name, color primary,
    entity model, byte dbg_log)
{
    return spawn_realm_block_baked_model(world, prefab, parent, seed, name, primary, model, 0, dbg_log);
}
