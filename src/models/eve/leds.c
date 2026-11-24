// a leds node: lights chasing along the box's 12 edges (its outer voxels), every Count'th voxel along each lit with
//  its NodeVoxel, a step further along each frame (the model's ModelFrame in its VoxAnimation): with Count frames
//  they go round and back to the start
//  - only on solid voxels (an edge it has), the edges' outermost voxels

static inline void leds_paint(VoxelNode* voxels, byte depth, byte3 p, byte voxel) {
    if (getv_VoxelNode(voxels, depth, p)) {
        set_VoxelNode(voxels, depth, p, voxel);
    }
}

void model_leds_node_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!zox_valid(node) || !zox_has(node, NodeType) || zox_getv(node, NodeType) != zox_model_node_leds) {
            continue;
        }
        const entity model = zox_get_link(world, e, ModelLink);
        if (zox_valid(model) && zox_has(model, VoxelNode) && zox_has(model, NodeDepth)) {
            const byte depth = zox_getv(model, NodeDepth);
            const int length = octree_size(depth);
            const byte voxel = zox_getv(node, NodeVoxel);
            const int spacing = int_max(1, zox_getv(node, Count));
            const int frame = zox_has(model, ModelFrame) ? zox_getv(model, ModelFrame) : 0;
            const byte last = (byte) (length - 1);
            spinlock* lock = zox_octree_lock(world, model);
            zox_octree_lock_begin(lock);
            VoxelNode* voxels = zox_getm(model, VoxelNode);
            for (int k = 0; k < length; k++) {
                if ((k + frame) % spacing) {
                    continue;
                }
                const byte b = (byte) k;
                for (byte corner = 0; corner < 4; corner++) {
                    const byte u = corner & 1 ? last : 0, v = corner & 2 ? last : 0;
                    leds_paint(voxels, depth, (byte3) { b, u, v }, voxel);
                    leds_paint(voxels, depth, (byte3) { u, b, v }, voxel);
                    leds_paint(voxels, depth, (byte3) { u, v, b }, voxel);
                }
            }
            zox_octree_lock_end(lock);
        }
        zox_add(e, TriggerEnd);
    }
}
