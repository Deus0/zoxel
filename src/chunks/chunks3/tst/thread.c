#define ZOX_TST_CHUNKS3_SAFETY_USE_LOCK 1

static void zox_tst_chunks3_safety_read(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        entity e = it->entities[i];
        VoxelNode* vnode = zox_getm(e, VoxelNode);
#if ZOX_TST_CHUNKS3_SAFETY_USE_LOCK
        VoxelNodeLock* lock = zox_getm(e, VoxelNodeLock);
        spin_lock(&lock->value);
#endif
        for (int j = 0; j < 100000; j++) {
            byte depth = 6;
            byte size = 1 << depth;
            byte3 position = {
                (byte)((j * 17) & (size - 1)),
                (byte)((j * 31) & (size - 1)),
                (byte)((j * 47) & (size - 1))
            };
            const VoxelNode* result =
            get_VoxelNode(
                vnode,
                depth,
                position);
            if (result) {
                volatile byte value = result->value;
                (void)value;
            }
        }
#if ZOX_TST_CHUNKS3_SAFETY_USE_LOCK
        spin_unlock(&lock->value);
#endif
    }
}

static void zox_tst_chunks3_safety_write(iter* it) {
    zox_sys_world();

    for (int i = 0; i < it->count; i++) {
        entity e = it->entities[i];

        VoxelNode* vnode = zox_getm(e, VoxelNode);
        VoxelNodeLock* lock = zox_getm(e, VoxelNodeLock);

        #if ZOX_TST_CHUNKS3_SAFETY_USE_LOCK
        spin_lock(&lock->value);
        #endif

        for (int j = 0; j < 10000; j++) {
            byte3 position = {
                (byte)(j & 31),
                (byte)((j >> 5) & 31),
                (byte)((j >> 10) & 31)
            };
            set_VoxelNode(
                vnode->ptr,
                5,
                position,
                (byte)(1 + (j & 7)));
        }

        #if ZOX_TST_CHUNKS3_SAFETY_USE_LOCK
        spin_unlock(&lock->value);
        #endif
    }
}

static void zox_tst_chunks3_safety_optimize(iter* it) {
    zox_sys_world();

    for (int i = 0; i < it->count; i++) {
        entity e = it->entities[i];

        VoxelNode* vnode = zox_getm(e, VoxelNode);
        VoxelNodeLock* lock = zox_getm(e, VoxelNodeLock);

        #if ZOX_TST_CHUNKS3_SAFETY_USE_LOCK
        spin_lock(&lock->value);
        #endif

        for (int j = 0; j < 1000; j++) {
            optimize_reduce_octree_node(
                vnode->ptr,
                sizeof(VoxelNode),
                offsetof(VoxelNode, value),
                offsetof(VoxelNode, type));
        }

        #if ZOX_TST_CHUNKS3_SAFETY_USE_LOCK
        spin_unlock(&lock->value);
        #endif
    }
}

// NOTE: Test the chunk voxels read/write safety
void zox_tst_chunks3_safety(ecs* world) {
    const int entity_count = 512;

    ECS_TAG(world, Chunks3SafetyRead);
    ECS_TAG(world, Chunks3SafetyWrite);
    ECS_TAG(world, Chunks3SafetyOptimize);

    entity* entities =
    malloc(sizeof(entity) * entity_count);

    for (int i = 0; i < entity_count; i++) {
        entity e = ecs_new(world);

        zox_setv(e, VoxelNode, 0);
        zox_setv(e, VoxelNodeLock, 0);
        zox_setv(e, NodeDepth, 6);

        VoxelNode* vnode =
        zox_getm(
            e,
            VoxelNode);

        for (int j = 0; j < 512; j++) {
            byte3 position = {
                (byte)(j & 15),
                (byte)((j >> 4) & 15),
                (byte)((j >> 8) & 15)
            };
            set_VoxelNode(
                vnode->ptr,
                6,
                position,
                (byte)(1 + (j & 7)));
        }
        ecs_add_id(
            world,
            e,
            ecs_id(Chunks3SafetyRead));

        ecs_add_id(
            world,
            e,
            ecs_id(Chunks3SafetyWrite));

        ecs_add_id(
            world,
            e,
            ecs_id(Chunks3SafetyOptimize));
        entities[i] = e;
    }

    entity read_system =
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "Chunks3SafetyRead"
        }),
        .phase = EcsOnUpdate,
        .query.terms = {
            {
                .id = ecs_id(Chunks3SafetyRead),
                .inout = EcsIn
            }
        },
        .callback = zox_tst_chunks3_safety_read,
        .multi_threaded = true
    });

    entity write_system =
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "Chunks3SafetyWrite"
        }),
        .phase = EcsOnUpdate,
        .query.terms = {
            {
                .id = ecs_id(Chunks3SafetyWrite),
                .inout = EcsIn
            }
        },
        .callback = zox_tst_chunks3_safety_write,
        .multi_threaded = true
    });

    entity optimize_system =
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "Chunks3SafetyOptimize"
        }),
        .phase = EcsOnUpdate,
        .query.terms = {
            {
                .id = ecs_id(Chunks3SafetyOptimize),
                .inout = EcsIn
            }
        },
        .callback = zox_tst_chunks3_safety_optimize,
        .multi_threaded = true
    });

    for (int i = 0; i < 64; i++) {
        ecs_progress(world, 0);
    }

    // Detect Corruption:
    //
    for (int i = 0; i < entity_count; i++) {
        entity e = entities[i];

        if (!ecs_is_alive(world, e)) {
            zox_loge(
                "[Chunks3Safety] Entity died [%i]",
                i);
            continue;
        }

        VoxelNode* vnode =
        zox_getm(
            e,
            VoxelNode);

        if (!vnode || !vnode->ptr) {
            zox_loge(
                "[Chunks3Safety] Invalid VoxelNode [%i]",
                i);
            continue;
        }

        byte max =
        get_octree_max_value(
            vnode->ptr,
            sizeof(VoxelNode),
            offsetof(VoxelNode, value));

        if (max > 8) {
            zox_loge(
                "[Chunks3Safety] CORRUPTION [%i] max=%u",
                i,
                max);
        }
    }

    zox_log(
        "[Chunks3Safety] complete lock=%i",
        ZOX_TST_CHUNKS3_SAFETY_USE_LOCK);

    for (int i = 0; i < entity_count; i++) {
        if (ecs_is_alive(world, entities[i])) {
            ecs_delete(world, entities[i]);
        }
    }

    free(entities);

    ecs_delete(world, read_system);
    ecs_delete(world, write_system);
    ecs_delete(world, optimize_system);

    /*
     * Flush system deletion so their queries are destroyed before
     * deleting the tag IDs used by those queries.
     */
    ecs_progress(world, 0);

    ecs_delete(world, Chunks3SafetyRead);
    ecs_delete(world, Chunks3SafetyWrite);
    ecs_delete(world, Chunks3SafetyOptimize);
}
