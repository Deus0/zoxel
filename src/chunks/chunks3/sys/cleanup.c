static inline byte optimize_reduce_octree_node(
    void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    if (!node) {
        return 0;
    }
    void** ptr = (void**) node;
    if (type_offset) {
        byte type = *(byte*)((char*) node + type_offset);
        if (type == node_type_instance) {
            return 0;
        }
    }
    void* kids = *ptr;
    // Leaf
    if (!kids) {
        return 1;
    }

    // First recurse into children.
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        optimize_reduce_octree_node(
            child,
            stride,
            value_offset,
            type_offset
        );
    }

    // Count child values.
    byte values[8];
    byte counts[8] = { 0 };
    byte value_count = 0;

    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);

        byte j;
        for (j = 0; j < value_count; j++) {
            if (values[j] == value) {
                counts[j]++;
                break;
            }
        }

        if (j == value_count) {
            values[value_count] = value;
            counts[value_count] = 1;
            value_count++;
        }
    }

    // Find mode.
    byte mode_value = 0;
    byte mode_count = 0;

    for (byte i = 0; i < value_count; i++) {
        if (type_offset && values[i] == 0) {
            continue;
        }

        if (counts[i] > mode_count) {
            mode_count = counts[i];
            mode_value = values[i];
        }
    }

    *(byte*)((char*) node + value_offset) = mode_value;

    // Check whether all children are identical leaves.
    byte first_value =
    *(byte*)((char*) kids + value_offset);

    byte uniform = 1;

    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;

        if (type_offset) {
            byte type =
            *(byte*)((char*) child + type_offset);

            if (type == node_type_instance) {
                uniform = 0;
                break;
            }
        }

        byte value =
        *(byte*)((char*) child + value_offset);

        void* subkids = *(void**) child;

        if (value != first_value || subkids != NULL) {
            uniform = 0;
            break;
        }
    }

    if (uniform) {
        *(byte*)((char*) node + value_offset) = first_value;
        zox_free(kids);
        *ptr = NULL;
        return 1;
    }

    return 0;
}

// TODO: Pass in CanGroup block byte tags, so we dont group some blocks like Grass
// NOTE: We might need a post optimize tag - for mesh updates etc
void voxel_octree_optimize_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeLock);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(VoxelNode, voxels);
        zox_sys_o(VoxelNodeLock, lock);
        spin_lock(&lock->value);
        optimize_reduce_octree_node(
            voxels,
            sizeof(VoxelNode),
            offsetof(VoxelNode, value),
            offsetof(VoxelNode, type));
        spin_unlock(&lock->value);
        zox_add(e, VoxelNodePostDirty);
        zox_remove(e, VoxelNodeDirty);
    }
    zox_sys_on_end();
} zoxd_system(voxel_octree_optimize_system);

zox_sys2(VoxelNodePostDirtySystem) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, VoxelNodePostDirty);
    }
} zox_sys_end(VoxelNodePostDirtySystem);
