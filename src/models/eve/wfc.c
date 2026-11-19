// a vox's voxels as a grid (ChunkSize, x fastest), for wfc_generate; free it after
byte* model_voxels_grid(ecs* world, entity vox, int3* size) {
    const byte depth = zox_getv(vox, NodeDepth);
    const int length = octree_size(depth);
    const int3 chunk = zox_getv(vox, ChunkSize);
    *size = (int3) { int_min(chunk.x, length), int_min(chunk.y, length), int_min(chunk.z, length) };
    byte* grid = malloc(size->x * size->y * size->z);
    const VoxelNode* tree = zox_get(vox, VoxelNode);
    for (int z = 0; z < size->z; z++)
    for (int y = 0; y < size->y; y++)
    for (int x = 0; x < size->x; x++) {
        grid[x + y * size->x + z * size->x * size->y] = getv_VoxelNode(tree, depth, (byte3) { x, y, z });
    }
    return grid;
}

// a model made of its source's patches, with its source's colours
//  - box: Shape3Size in the model's voxels, scaled by the size node's ModelSize (its ratio of the blueprint's length)
void process_node_model_wfc(ecs* world, entity node, entity source, entity model, byte3 model_size) {
    byte3 position = byte3_zero;
    byte3 box = zox_getv(node, Shape3Size);
    if (!byte3_equals(model_size, byte3_zero)) {
        scale_node_transform(model_size, &position, &box);
    }
    const byte depth = zox_getv(model, NodeDepth);
    const int length = octree_size(depth);
    const int3 size = { int_min(box.x, length), int_min(box.y, length), int_min(box.z, length) };
    int3 source_size;
    byte* source_grid = model_voxels_grid(world, source, &source_size);
    byte* grid = malloc(size.x * size.y * size.z);
    const uint seed = (uint) zox_getv(model, Seed);
    if (!wfc_generate(source_grid, source_size, grid, size, seed, zox_getv(node, WfcRules), wfc_patch_size)) {
        zox_logw("[wfc] no model from [%s] in [%ix%ix%i] for seed [%u]", zox_getn(source), size.x, size.y, size.z, seed);
    }
    zox_muter(model, VoxelNode, tree);
    spinlock* write_lock = zox_octree_lock(world, model);
    zox_octree_lock_begin(write_lock);
    for (int z = 0; z < size.z; z++)
    for (int y = 0; y < size.y; y++)
    for (int x = 0; x < size.x; x++) {
        const byte voxel = grid[x + y * size.x + z * size.x * size.y];
        if (voxel) {
            set_VoxelNode(tree, depth, (byte3) { x, y, z }, voxel);
        }
    }
    zox_octree_lock_end(write_lock);
    const ColorRGBs* source_colors = zox_get(source, ColorRGBs);
    zox_muter(model, ColorRGBs, colors);
    resize_ColorRGBs(colors, source_colors->length);
    memcpy(colors->value, source_colors->value, source_colors->length * sizeof(color_rgb));
    free(source_grid);
    free(grid);
}

// a wfc node: its process's model becomes a new one, made from the node's source model (ModelLink)
void model_wfc_node_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!zox_valid(node) || !zox_has(node, NodeType) || zox_getv(node, NodeType) != zox_model_node_wfc) {
            continue;
        }
        const entity model = zox_get_link(world, e, ModelLink);
        const entity source = zox_get_link(world, node, ModelLink);
        if (!zox_valid(model) || !zox_valid(source) || !zox_has(source, VoxelNode) || !zox_has(model, VoxelNode)) {
            zox_logw("[wfc] node [%s] has no model or source", zox_getn(node));
        } else {
            process_node_model_wfc(world, node, source, model, zox_getv(e, ModelSize));
        }
        zox_add(e, TriggerEnd);
    }
}
