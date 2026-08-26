void set_as_debug_vox(ecs *world, entity e) {
    zox_muter(e, ColorRGBs, colorRGBs);
    resize_memory_component(ColorRGBs, colorRGBs, color_rgb, 1);
    colorRGBs->value[0] = (color_rgb) { 223, 239, 2 };
    zox_set(e, ChunkSize, { { 1, 1, 1 } });
    zox_muter(e, VoxelNode, voxelNode);
    fill_octree(voxelNode, 1, 1);
}

byte is_vox_valid(const vox_file *vox) {
    return (vox && vox->chunks);
}

byte pick_node_depth(int3 size) {
    int max_dim = int_max(size.x, int_max(size.y, size.z));
    byte depth = 0;
    while ((1 << depth) < max_dim) {
        depth++;
    }
    return depth;
}

// TODO: we should pick the depth that matches the size of the model
//      - then we scale the model to fit our world
void set_vox_file(
    ecs *world,
    entity e,
    const vox_file* vox,
    byte reducer,
    float bscale)
{
    if (!is_vox_valid(vox)) {
        zox_log_error("error reading voxfile");
        set_as_debug_vox(world, e);
        return;
    }
    byte reduction_length = powers_of_two[reducer];
    const byte* voxels = vox->chunks[0].xyzi.voxels;
    int3 size = vox->chunks[0].size.xyz;
    byte3 ogsize = int3_to_byte3(size);
    byte node_depth = pick_node_depth(size);
    if (node_depth - reducer <= 0) {
        return;
    }
    node_depth -= reducer;
    int length = powers_of_two[node_depth];
    // int max_dim = int_max(size.x, int_max(size.y, size.z));
    int3 rsize = size;  // reduce our ogsize too by shrinkiing
    rsize.x /= reduction_length;
    rsize.y /= reduction_length;
    rsize.z /= reduction_length;
    size = int3_single(length);
    // REMEMBER: Offset is broken atm, i think build mesh still checks the bounds
    //      And we are just offsetting from the corner anyway
    /*zox_log("Setting VoxFile:");
    zox_log("   - reducer [%i]", reducer);
    zox_log("   - node_depth [%i]", node_depth);
    zox_log("   - length [%i]", length);
    zox_log("   - scale [%f]", scale);
    zox_log("   - size(og) [%ix%ix%i]", ogsize.x, ogsize.y, ogsize.z);
    zox_log("   - size(reduced) [%ix%ix%i]", rsize.x, rsize.y, rsize.z);
    zox_log("   - offset [%ix%ix%i]", offset.x, offset.y, offset.z);*/
    zox_set(e, BlockScale, { bscale });
    zox_set(e, NodeDepth, { node_depth });
    zox_set(e, ChunkSize, { rsize });
    zox_muter(e, VoxelNode, node);
    // wheres our offset for our vox model into a new grid??
    byte3 position;
    int vox_index;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                // at least bound it there
                if (position.x >= ogsize.x || position.y >= ogsize.y || position.z >= ogsize.z) {
                    continue;
                }
                // we need to adjust our position GET too
                byte3 gpos = position;
                gpos.x *= reduction_length;
                gpos.y *= reduction_length;
                gpos.z *= reduction_length;
                if (gpos.x >= ogsize.x || gpos.y >= ogsize.y || gpos.z >= ogsize.z) {
                    continue;
                }
                vox_index = byte3_array_index(gpos, ogsize);

                if (!voxels[vox_index]) {
                    continue;
                }
                byte3 ppos = position;
                set_VoxelNode(node, node_depth, ppos, voxels[vox_index]);
            }
        }
    }
    zox_add(e, VoxelNodeDirty);
    // Copy our file into our colors
    int clength = vox->palette.values_length;
    zox_muter(e, ColorRGBs, colors);
    resize_ColorRGBs(colors, clength);
    memcpy(colors->value, vox->palette.values_rgb, clength * sizeof(color_rgb));
}

// TODO: Convert vox_file to VoxNode, and clone to depth to ModelLods
//      atm we rebuild everytime the same
entity spawn_vox_file(
    ecs *world,
    entity prefab,
    const vox_file* data,
    const char* filename)
{
    entity model = zox_new();
    {
        char name[128];
        sprintf(name, "vox_file_%s", filename);
        zox_set_unique_name(model, name);
    }
    byte mdepth = pick_node_depth(data->chunks[0].size.xyz);
    zox_set(model, MaxRenderDepth, { mdepth });
    ModelLods model_lods;
    for (byte rdepth = 0; rdepth <= mdepth; rdepth++) {
        byte reducer = mdepth - rdepth;
        float bscale = (1 / 64.0f);
        bscale *= powers_of_two[mdepth - rdepth];
        entity e2 = zox_ins(world, prefab);
        {
            char name[128];
            sprintf(name, "vox_file_lod_%s", filename);
            zox_set_unique_name(e2, name);
        }
        set_vox_file(world, e2, data, reducer, bscale);
        zox_add(e2, BuildMesh);
        zox_set(e2, RenderDepth, { rdepth });
        zox_set(e2, MaxRenderDepth, { mdepth });
        model_lods.value[rdepth] = e2;
    }
    zox_set_ptr(model, ModelLods, model_lods);
    return model;
}
