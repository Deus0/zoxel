// NOTE: A/B Testing to proove locks work for VoxelNode
// 0 = Crashes | 1 = Works
byte zoxt_chunks3_safety_locks = 1;
// NOTE: No fields, as that creates chain dependencies
#define zoxt_chunks3_safety_fields 0
// NOTE: Single needs to be on to ensure it does it at same time
#define zoxt_chunks3_safety_single 1
entity zoxt_chunks3_safety_single_entity;
// NOTE: used mostly to ensure queries and division is done
#define zoxt_chunks3_safety_entities 4 // 64
#define zoxt_chunks3_safety_threads 4
#define zoxt_chunks3_safety_iterations 4
#define zoxt_chunks3_safety_reads 1000
#define zoxt_chunks3_safety_writes 1000
#define zoxt_chunks3_safety_culls 1000

static inline void zox_sys_log_thread(
    iter* it,
    const char* label)
{
    zox_log("[%s] thread=%lu count=%i first=%llu last=%llu",
        label,
        (unsigned long) pthread_self(),
        it->count,
        (unsigned long long)it->entities[0],
        (unsigned long long)it->entities[it->count - 1]);
}

static void zox_tst_chunks3_safety_read(iter* it) {
    zox_sys_log_thread(it, "chunks3_safety_read");
#if zoxt_chunks3_safety_fields
    zox_sys_begin();
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeLock);
#else
    zox_sys_world();
#endif
    for (int i = 0; i < it->count; i++) {
#if zoxt_chunks3_safety_fields
        zox_sys_o(VoxelNode, vnode);
        zox_sys_o(VoxelNodeLock, lock);
#else
        zox_sys_e();
#if zoxt_chunks3_safety_single
        e = zoxt_chunks3_safety_single_entity;
#endif
        VoxelNode* vnode = zox_getm(e, VoxelNode);
        VoxelNodeLock* lock = zox_getm(e, VoxelNodeLock);
#endif
        for (int j = 0; j < zoxt_chunks3_safety_reads; j++) {
            byte depth = 6;
            byte size = 1 << depth;
            byte3 position = {
                (byte)((j * 17) & (size - 1)),
                (byte)((j * 31) & (size - 1)),
                (byte)((j * 47) & (size - 1))
            };
            if (zoxt_chunks3_safety_locks) {
                spin_lock(&lock->value);
            }
            const VoxelNode* result =
            get_VoxelNode(
                vnode,
                depth,
                position);
            if (result) {
                volatile byte value = result->value;
                (void)value;
            }
            if (zoxt_chunks3_safety_locks) {
                spin_unlock(&lock->value);
            }
        }
    }
} zoxd_system(zox_tst_chunks3_safety_read);

// NOTE: Sets random values for nodes
static void zox_tst_chunks3_safety_write(iter* it) {
    zox_sys_log_thread(it, "chunks3_safety_writes");
#if zoxt_chunks3_safety_fields
    zox_sys_begin();
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeLock);
#else
    zox_sys_world();
#endif
    for (int i = 0; i < it->count; i++) {
#if zoxt_chunks3_safety_fields
        zox_sys_o(VoxelNode, vnode);
        zox_sys_o(VoxelNodeLock, lock);
#else
        zox_sys_e();
#if zoxt_chunks3_safety_single
        e = zoxt_chunks3_safety_single_entity;
#endif
        VoxelNode* vnode = zox_getm(e, VoxelNode);
        VoxelNodeLock* lock = zox_getm(e, VoxelNodeLock);
#endif
        for (int j = 0; j < zoxt_chunks3_safety_writes; j++) {
            byte3 position = {
                (byte)(j & 31),
                (byte)((j >> 5) & 31),
                (byte)((j >> 10) & 31)
            };
            if (zoxt_chunks3_safety_locks) {
                spin_lock(&lock->value);
            }
            set_VoxelNode(
                vnode,
                5,
                position,
                (byte)(1 + (j & 7)));
            if (zoxt_chunks3_safety_locks) {
                spin_unlock(&lock->value);
            }
        }
    }
} zoxd_system(zox_tst_chunks3_safety_write);

static void zox_tst_chunks3_safety_cull(iter* it) {
    zox_sys_log_thread(it, "chunks3_safety_culls");
#if zoxt_chunks3_safety_fields
    zox_sys_begin();
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeLock);
#else
    zox_sys_world();
#endif
    for (int i = 0; i < it->count; i++) {
#if zoxt_chunks3_safety_fields
        zox_sys_o(VoxelNode, vnode);
        zox_sys_o(VoxelNodeLock, lock);
#else
        zox_sys_e();
#if zoxt_chunks3_safety_single
        e = zoxt_chunks3_safety_single_entity;
#endif
        VoxelNode* vnode = zox_getm(e, VoxelNode);
        VoxelNodeLock* lock = zox_getm(e, VoxelNodeLock);
#endif
        for (int j = 0; j < zoxt_chunks3_safety_culls; j++) {
            byte depth = 1 + (byte)(j & 5);
            byte size = 1 << depth;

            byte3 position = {
                (byte)(((j * 17) ^ (j >> 3)) & (size - 1)),
                (byte)(((j * 31) ^ (j >> 5)) & (size - 1)),
                (byte)(((j * 47) ^ (j >> 7)) & (size - 1))
            };
            if (zoxt_chunks3_safety_locks) {
                spin_lock(&lock->value);
            }
            VoxelNode* branch = getm_VoxelNode(
                vnode,
                position,
                depth);

            if (branch) {
                octree_collapse(
                    branch,
                    sizeof(VoxelNode));
            }
            if (zoxt_chunks3_safety_locks) {
                spin_unlock(&lock->value);
            }
        }
    }
} zoxd_system(zox_tst_chunks3_safety_cull);

zox_tag(Chunks3SafetyRead);
zox_tag(Chunks3SafetyWrite);
zox_tag(Chunks3SafetyOptimize);

void test_system(
    ecs* world,
    const char* name,
    entity tag,
    ecs_iter_action_t callback)
{
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = name
        }),
        .phase = EcsOnUpdate,
        .query.terms = {
#if zoxt_chunks3_safety_fields
            {
                .id = zox_id(VoxelNode),
                .inout = EcsOut
            },
            {
                .id = zox_id(VoxelNodeLock),
                .inout = EcsOut
            },
#endif
            {
                .id = tag,
                .inout = EcsIn
            },
        },
        .callback = callback,
        .multi_threaded = 1
    });
}

// NOTE: Test the chunk voxels read/write safety
void zox_tst_chunks3_safety(ecs* zox_world) {
    (void) zox_world;
    // spawn our test ecs data
    ecs* world = ecs_init();
    ecs_set_threads(world, zoxt_chunks3_safety_threads);
    zoxd(VoxelNode);
    zoxd(NodeDepth);
    zoxd(VoxelNodeLock);
    zoxd_tag(Chunks3SafetyRead);
    zoxd_tag(Chunks3SafetyWrite);
    zoxd_tag(Chunks3SafetyOptimize);
    // Initialize Systems
    /*test_system(
        world,
        "zox_tst_chunks3_safety_read",
        zox_id(Chunks3SafetyRead),
        zox_tst_chunks3_safety_read);
    test_system(
        world,
        "zox_tst_chunks3_safety_write",
        zox_id(Chunks3SafetyWrite),
        zox_tst_chunks3_safety_write);
    test_system(
        world,
        "zox_tst_chunks3_safety_optimize",
        zox_id(Chunks3SafetyOptimize),
        zox_tst_chunks3_safety_optimize);*/
    zox_system(
        zox_tst_chunks3_safety_read,
        EcsOnUpdate,
#if zoxt_chunks3_safety_fields
        [out] VoxelNode,
        [out] VoxelNodeLock,
#endif
        [none] Chunks3SafetyRead,
    );
    zox_system(
        zox_tst_chunks3_safety_write,
        EcsOnUpdate,
#if zoxt_chunks3_safety_fields
        [out] VoxelNode,
        [out] VoxelNodeLock,
#endif
        [none] Chunks3SafetyWrite,
    );
    zox_system(
        zox_tst_chunks3_safety_cull,
        EcsOnUpdate,
#if zoxt_chunks3_safety_fields
        [out] VoxelNode,
        [out] VoxelNodeLock,
#endif
        [none] Chunks3SafetyOptimize,
    );
    // Initialize Entities
    entity* entities = malloc(
        sizeof(entity) *
        zoxt_chunks3_safety_entities);
    for (int i = 0; i < zoxt_chunks3_safety_entities; i++) {
        entity e = zox_new();
        zox_setv(e, VoxelNode, 0);
        zox_setv(e, NodeDepth, 6);
        zox_add(e, Chunks3SafetyWrite);
        zox_add(e, Chunks3SafetyRead);
        zox_add(e, Chunks3SafetyOptimize);
        spinlock vlock;
        spinlock_init(&vlock);
        zox_setv(e, VoxelNodeLock, vlock);
        VoxelNode* vnode = zox_getm(e, VoxelNode);
        for (int j = 0; j < 512; j++) {
            byte3 position = {
                (byte) (j & 15),
                (byte) ((j >> 4) & 15),
                (byte) ((j >> 8) & 15)
            };
            set_VoxelNode(
                vnode,
                6,
                position,
                (byte) (1 + (j & 7)));
        }
        entities[i] = e;
        if (i == 0) {
            zoxt_chunks3_safety_single_entity = e;
        }
    }
    // Test Loop
    zox_log("Running Test [chunks3_safety]");
    for (int i = 0; i < zoxt_chunks3_safety_iterations; i++) {
        zox_log("   - [chunks3_safety] [%i]", i);
        ecs_progress(world, 0);
    }
    // Detect Corruption
    zox_log("Checking Test Results [chunks3_safety]");
    for (int i = 0; i < zoxt_chunks3_safety_entities; i++) {
        entity e = entities[i];
        if (!ecs_is_alive(world, e)) {
            zox_loge("[Chunks3Safety] Entity died [%i]", i);
            continue;
        }
        VoxelNode* vnode = zox_getm(e, VoxelNode);
        if (!vnode) {
            zox_loge("[Chunks3Safety] Invalid VoxelNode [%i]", i);
            continue;
        }
        byte max =
            get_octree_max_value(
                vnode,
                sizeof(VoxelNode),
                offsetof(VoxelNode, value));
        if (max > 8) {
            zox_loge(
                "[Chunks3Safety] CORRUPTION [%i] max=%u",
                i,
                max);
        }
    }
    // Cleanup
    zox_log("[Chunks3Safety] Locks [%i]",
        zoxt_chunks3_safety_locks);
    ecs_fini(world);
    free(entities);
}
