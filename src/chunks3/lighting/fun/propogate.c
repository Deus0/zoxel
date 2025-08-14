static inline void flood_light_recursive(
    const VoxelNode* vnode,
    LightNode* lnode,
    const VoxelNode* voxel_neighbors[6],
    const LightNode* light_neighbors[6],
    byte depth,
    byte3 pos,
    byte light,
    LightNodeQueue* queue,
    int max_depth
) {
    if (light == 0 || max_depth <= 0) return;

    byte current = get_LightNode_value_ex(lnode, depth, pos, 0);
    if (current >= light) return;

    set_LightNode_ex(lnode, depth, pos, light, 0);

    byte next_light = (light > 32) ? light - 32 : 0;
    if (next_light == 0) return;

    for (byte dir = 0; dir < 6; dir++) {
        byte3 npos = pos;

        // move into neighbor voxel first
        switch (dir) {
            case 0: if (npos.x>0) npos.x--; break;
            case 1: if (npos.x<(1<<depth)-1) npos.x++; break;
            case 2: if (npos.y>0) npos.y--; break;
            case 3: if (npos.y<(1<<depth)-1) npos.y++; break;
            case 4: if (npos.z>0) npos.z--; break;
            case 5: if (npos.z<(1<<depth)-1) npos.z++; break;
        }

        const VoxelNode* neighbor_voxel = (VoxelNode*)octree_get_adjacent_leaf(
            vnode, (const void**)voxel_neighbors, dir, npos, depth, sizeof(VoxelNode), offsetof(VoxelNode, value));

        const LightNode* neighbor_light = (LightNode*)octree_get_adjacent_leaf(
            lnode, (const void**)light_neighbors, dir, npos, depth, sizeof(LightNode), offsetof(LightNode, value));

        if (!neighbor_voxel && !queue) continue; // outside chunk → skip or queue

        // skip if neighbor light >= next_light
        if (neighbor_light) {
            byte nlight = get_LightNode_value_ex(neighbor_light, depth, npos, 0);
            if (nlight >= next_light) continue;
        }

        // decay if solid voxel
        const VoxelNode* vn = neighbor_voxel ? get_VoxelNode_ex((VoxelNode*)neighbor_voxel, depth, npos, 0) : NULL;
        byte nl = next_light;
        if (vn && vn->value) nl = (next_light > 64) ? next_light - 64 : 0;
        if (nl == 0) continue;

        // outside chunk → queue
        if (!neighbor_voxel && queue) {
            // spin_lock(&queue->lock);
            a_LightNodeQueue(queue, (LightNodeUpdate){ .value = nl, .positionl = npos });
            // spin_unlock(&queue->lock);
        } else {
            flood_light_recursive(
                neighbor_voxel,
                lnode,
                voxel_neighbors,
                light_neighbors,
                depth,
                npos,
                nl,
                queue,
                max_depth - 1
            );
        }
    }
}
