// Octree locks: a reader locks an octree around walking it, as writes can open or close (free) its nodes
//  - see doc/thread_safety.md, readers and writers can share an op (no barrier between them)
//  - one at a time, or several with zox_lock_voxel_trees (address order, no deadlocks)
//  - the spinlock isn't reentrant: never lock one you hold

// an entity's octree lock, NULL if it has none
static inline spinlock* zox_octree_lock(ecs* world, entity e) {
    if (!zox_valid(e) || !zox_has(e, VoxelNodeLock)) {
        return NULL;
    }
    return &(zox_mut(e, VoxelNodeLock)->value);
}

static inline void zox_octree_lock_begin(spinlock* lock) {
    if (lock) {
        spin_lock(lock);
    }
}

static inline void zox_octree_lock_end(spinlock* lock) {
    if (lock) {
        spin_unlock(lock);
    }
}

// a chunk's and its 6 neighbors' locks, for zox_lock_voxel_trees
static inline void zox_octree_neighbor_locks(ecs* world, const ChunkNeighbors* neighbors, spinlock** locks) {
    for (byte i = 0; i < 6; i++) {
        locks[i] = neighbors ? zox_octree_lock(world, neighbors->value[i]) : NULL;
    }
}
