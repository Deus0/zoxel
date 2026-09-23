/*entity spawn_model(
    ecs *world,
    entity parent,
    const char* name,
    byte max_depth,
    byte depth,
   // byte3 rsize,
    lint seed,
    color vox_color)
{
    // entity prefab_lod = prefab_vox; // prefab_invisible_vox,
    entity e = spawn_vox_basic(
        world,
        prefab_vox,
        depth,
        max_depth);
    {
        char name2[64];
        sprintf(name2, "model_%s", name);
        zox_name(name2);
    }
    zox_add(e, VoxelNodeDirty);
    zox_setv(e, Seed, seed);
    zox_setv(e, Color, vox_color);
    zox_set_parent(world, e, parent);
    // zox_setv(e, ChunkSize, byte3_to_int3(rsize));
    // byte ddepth = max_depth - depth;
    // short length = octree_size(ddepth);
    // float bscale = ((float) length) / 64.0f;
    // zox_instance(prefab_vox);
    // zox_setv(e, NodeDepth, depth);
    // zox_setv(e, BlockScale, bscale);
    // initialize_voxel_lock(world, e);
    // initialize_sides_lock(world, e);
    // zox_set(e, GenerateModel, { zox_generate_model_run });
    // zox_setv(e, RenderDepth, depth);  // move this to prefab
    // zox_setv(e, MaxRenderDepth, max_depth);
    return e;
}

// TODO: Remove Model Lods and just use Models with children Render Meshes
entity2 spawn_model_lods(
    ecs* world,
    entity parent,
    color vox_color,
    lint seed,
    byte max_depth,
    byte3 rsize,
    const char* label)
{
    srand(seed);
    vox_color = color_mutate(vox_color, 40);
    entity e = zox_new();
    zox_name("model_lods");
    zox_add(e, Model);
    zox_setv(e, Seed, seed);
    zox_setv(e, MaxRenderDepth, max_depth);
    zox_set_parent(world, e, parent);
    entity vox = 0;
    ModelLods mlods = { 0 };
    for (ushort depth = 0; depth <= max_depth; depth++) {
        entity e2 = spawn_model(
            world,
            e,
            label,
            max_depth,
            depth,
            // rsized,
            seed,
            vox_color);
        // zox_set_unique_name(e2, label);
        if (depth == max_depth) {
            vox = e2;
        }
        mlods.value[depth] = e2;
    }
    zox_set_ptr(e, ModelLods, mlods);
    return (entity2) {
        e,
        vox
    };
}*/

/*short ddepth = octree_size(max_depth - depth);
 b yte3 rsized = rsize;*
 rsized.x /= ddepth;
 rsized.y /= ddepth;
 rsized.z /= ddepth;*/
