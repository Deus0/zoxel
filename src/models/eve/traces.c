// a traces node: Count circuit traces of its NodeVoxel over the model's surface, seeded by the model's Seed
//  - each starts on a face: a seeded spot on one of the box's 6 sides, marched in to the first solid voxel
//  - it walks along that face in straight runs (2 to a quarter of the model's length), turning a quarter between
//    them, painting each voxel it crosses while the voxel stays solid with air (or the box's side) over it
//  - a run that would leave the surface turns early, so it follows any shape, not only a cube's sides
#define traces_runs_min 3
#define traces_runs_max 6
#define traces_start_tries 16

static inline byte traces_solid(const VoxelNode* voxels, byte depth, int3 p) {
    const int length = octree_size(depth);
    if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
        return 0;
    }
    return getv_VoxelNode(voxels, depth, int3_to_byte3(p)) != 0;
}

// solid, with air (or outside the box) along the face's normal
static inline byte traces_on_face(const VoxelNode* voxels, byte depth, int3 p, int3 normal) {
    return traces_solid(voxels, depth, p) && !traces_solid(voxels, depth, int3_add(p, normal));
}

// a start on a seeded face: a seeded spot on it, marched in to the first solid voxel; 0 if the line's all air
static inline byte traces_start(const VoxelNode* voxels, byte depth, uint* seed, int3* start, int3* normal) {
    const int length = octree_size(depth);
    const byte face = seed_rand(*seed) % 6;
    *seed = seed_rand(*seed + 1);
    const byte axis = face / 2;
    const int sign = face % 2 ? 1 : -1;
    int3 p;
    *int3_axis(&p, axis == 0 ? 1 : 0) = (int) (seed_rand(*seed) % length);
    *int3_axis(&p, axis == 2 ? 1 : 2) = (int) (seed_rand(*seed + 1) % length);
    *seed = seed_rand(*seed + 2);
    *normal = int3_zero;
    *int3_axis(normal, axis) = sign;
    for (int d = 0; d < length; d++) {
        *int3_axis(&p, axis) = sign > 0 ? length - 1 - d : d;
        if (traces_solid(voxels, depth, p)) {
            *start = p;
            return 1;
        }
    }
    return 0;
}

static inline void traces_walk(VoxelNode* voxels, byte depth, byte voxel, uint* seed, int3 p, int3 normal) {
    const int length = octree_size(depth);
    const byte normal_axis = normal.x ? 0 : (normal.y ? 1 : 2);
    byte axis = normal_axis == 0 ? 1 : 0;
    const byte other = 3 - normal_axis - axis;
    const int runs = traces_runs_min + (int) (seed_rand(*seed) % (traces_runs_max - traces_runs_min + 1));
    set_VoxelNode(voxels, depth, int3_to_byte3(p), voxel);
    for (int r = 0; r < runs; r++) {
        *seed = seed_rand(*seed + 3);
        int3 step = int3_zero;
        *int3_axis(&step, axis) = seed_rand(*seed) % 2 ? 1 : -1;
        const int run = 2 + (int) (seed_rand(*seed + 1) % (uint) int_max(1, length / 4 - 1));
        for (int k = 0; k < run; k++) {
            const int3 next = int3_add(p, step);
            if (!traces_on_face(voxels, depth, next, normal)) {
                break;
            }
            p = next;
            set_VoxelNode(voxels, depth, int3_to_byte3(p), voxel);
        }
        axis = axis == other ? (normal_axis == 0 ? 1 : 0) : other;
    }
}

void model_traces_node_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!zox_valid(node) || !zox_has(node, NodeType) || zox_getv(node, NodeType) != zox_model_node_traces) {
            continue;
        }
        const entity model = zox_get_link(world, e, ModelLink);
        if (zox_valid(model) && zox_has(model, VoxelNode) && zox_has(model, NodeDepth)) {
            const byte depth = zox_getv(model, NodeDepth);
            const byte voxel = zox_getv(node, NodeVoxel);
            const int count = zox_getv(node, Count);
            uint seed = seed_rand((uint) zox_getv(model, Seed) + 4099 + voxel * 131u);
            spinlock* lock = zox_octree_lock(world, model);
            zox_octree_lock_begin(lock);
            VoxelNode* voxels = zox_getm(model, VoxelNode);
            for (int t = 0; t < count; t++) {
                int3 start, normal;
                for (int tries = 0; tries < traces_start_tries; tries++) {
                    if (traces_start(voxels, depth, &seed, &start, &normal)) {
                        traces_walk(voxels, depth, voxel, &seed, start, normal);
                        break;
                    }
                }
            }
            zox_octree_lock_end(lock);
        }
        zox_add(e, TriggerEnd);
    }
}
