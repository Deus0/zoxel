static inline byte reduce_max_octree_node(
    void* node,
    size_t stride,
    size_t value_offset)
{
    if (!node) {
        return 0;
    }

    void** ptr = (void**) node;
    void* kids = *ptr;

    // Leaf.
    if (!kids) {
        return *(byte*)((char*) node + value_offset);
    }

    byte max_value = 0;

    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;

        byte child_max = reduce_max_octree_node(
            child,
            stride,
            value_offset
        );

        if (child_max > max_value) {
            max_value = child_max;
        }
    }

    // Always update this node from its children.
    *(byte*)((char*) node + value_offset) = max_value;

    // Collapse only when every child is a leaf
    // and every child has the same value.
    byte can_collapse = 1;

    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;

        if (*(void**) child != NULL) {
            can_collapse = 0;
            break;
        }

        byte value = *(byte*)((char*) child + value_offset);
        if (value != max_value) {
            can_collapse = 0;
            break;
        }
    }
    if (can_collapse) {
        free(kids);
        *ptr = NULL;
    }
    return max_value;
}

// Groups same nodes when dirty
zox_sys2(LightNodeReduceSystem) {
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_out(LightNodeLock);
    zox_sys_out(LightNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LightNodeDirty, dirty);
        zox_sys_o(LightNodeLock, lightlock);
        zox_sys_o(LightNode, lights);
        if (dirty->value == zox_dirty_active) {
            if (locks_enabled) {
                spin_lock(&lightlock->value);
            }
            reduce_max_octree_node(
                lights,
                sizeof(LightNode),
                offsetof(LightNode, value));
            if (locks_enabled) {
                spin_unlock(&lightlock->value);
            }
        }
    }
} zox_sys_end(LightNodeReduceSystem);