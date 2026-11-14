// a vertex's bones from its vox's voxel parts (BoneNode), doc/done/skeletons.md
//  - a face's bone: its voxel's (half a voxel behind its middle, the solid side) part's bone (CombineBones)
//  - a joint: a vertex whose corner touches voxels of a bone joined to the face's (its parent or child) blends toward it
//    by how many of the 8 it is, so where a chest meets a shoulder both sides meet halfway; an arm resting on the chest
//    isn't joined to it and stays its own
//  - out: { bone, other bone, its weight 0-255, 0 }
typedef struct {
    const byte* grid;   // each voxel's bone at depth (255 air), flattened from BoneNode once a mesh
    const CombineBones* table;
    byte depth;
    int length;         // voxels a side at depth
    float size;         // a voxel at depth
    float3 bounds;
    const byte* parents;
    int bones;
} bone_parts;

// the bone of the voxel at a grid position, 255 if air or outside
static inline byte bone_parts_at(const bone_parts* b, int3 g) {
    if (g.x < 0 || g.y < 0 || g.z < 0 || g.x >= b->length || g.y >= b->length || g.z >= b->length) {
        return 255;
    }
    return b->grid[(g.x * b->length + g.y) * b->length + g.z];
}

// BoneNode's bones into a grid at depth (an octree walk a lookup was 31ms a body, 8 a vertex)
static inline void bone_parts_fill(byte* grid, int length, const CombineBones* table, const BoneNode* node, byte depth,
    byte target, int3 position)
{
    if (depth < target && has_children_BoneNode(node)) {
        const BoneNode* kids = get_children_BoneNode(node);
        for (byte i = 0; i < octree_length; i++) {
            const byte3 o = octree_positions[i];
            bone_parts_fill(grid, length, table, &kids[i], depth + 1, target,
                (int3) { position.x * 2 + o.x, position.y * 2 + o.y, position.z * 2 + o.z });
        }
        return;
    }
    const byte bone = !node->value ? 255 : node->value - 1 < table->length ? table->value[node->value - 1] : 0;
    const int span = 1 << (target - depth);
    const int3 start = { position.x * span, position.y * span, position.z * span };
    for (int x = start.x; x < start.x + span; x++)
    for (int y = start.y; y < start.y + span; y++) {
        memset((byte*) grid + (x * length + y) * length + start.z, bone, span);
    }
}

static inline int3 bone_parts_grid(const bone_parts* b, float3 p) {
    return (int3) { (int) floorf((p.x + b->bounds.x) / b->size), (int) floorf((p.y + b->bounds.y) / b->size),
        (int) floorf((p.z + b->bounds.z) / b->size) };
}

static inline byte bone_parts_joined(const bone_parts* b, byte x, byte y) {
    return x < b->bones && y < b->bones && (b->parents[x] == y || b->parents[y] == x);
}

// a face's bone: its voxel, behind its middle on whichever side is solid
static inline byte bone_parts_face(const bone_parts* b, const float3* v) {
    const float3 c = float3_scale(float3_add(float3_add(v[0], v[1]), float3_add(v[2], v[3])), 0.25f);
    const float3 n = float3_normalize(float3_cross(float3_sub(v[1], v[0]), float3_sub(v[2], v[0])));
    const float h = b->size * 0.5f;
    const byte front = bone_parts_at(b, bone_parts_grid(b, float3_add(c, float3_scale(n, h))));
    const byte back = bone_parts_at(b, bone_parts_grid(b, float3_sub(c, float3_scale(n, h))));
    return back != 255 ? back : front != 255 ? front : 0;
}

// a vertex: its face's bone, blended toward a joined bone among its corner's 8 voxels
static inline byte4 bone_parts_vertex(const bone_parts* b, float3 p, byte bone) {
    const int3 k = { (int) roundf((p.x + b->bounds.x) / b->size), (int) roundf((p.y + b->bounds.y) / b->size),
        (int) roundf((p.z + b->bounds.z) / b->size) };
    byte others[8], counts[8], kinds = 0, own = 0, best = 0;
    for (int i = 0; i < 8; i++) {
        const byte at = bone_parts_at(b, (int3) { k.x - 1 + (i & 1), k.y - 1 + ((i >> 1) & 1), k.z - 1 + ((i >> 2) & 1) });
        if (at == bone) {
            own++;
            continue;
        }
        if (at == 255 || !bone_parts_joined(b, bone, at)) {
            continue;
        }
        byte j = 0;
        while (j < kinds && others[j] != at) {
            j++;
        }
        if (j == kinds) {
            others[kinds] = at;
            counts[kinds++] = 0;
        }
        counts[j]++;
        best = counts[j] > counts[best] ? j : best;
    }
    if (!kinds) {
        return (byte4) { bone, bone, 0, 0 };
    }
    return (byte4) { bone, others[best], (byte) (255 * counts[best] / (own + counts[best])), 0 };
}

// every vertex's bones, 4 a face (build_vox_mesh_at_depth), 0 if the vox has no parts
byte generate_weights_parts(ecs* world, entity vox, byte depth, const MeshVertices* verts, const BoneLinks* bones, byte4* out) {
    if (!zox_valid(vox) || !zox_has(vox, BoneNode) || !zox_has(vox, CombineBones) || verts->length % 4) {
        return 0;
    }
    byte parents[bones->length];
    for (int k = 0; k < bones->length; k++) {
        const entity parent = zox_get_parent(world, bones->value[k]);
        parents[k] = 255;
        for (int j = 0; j < bones->length; j++) {
            if (bones->value[j] == parent) {
                parents[k] = j;
            }
        }
    }
    const byte node_depth = zox_getv(vox, NodeDepth);
    const float block_scale = zox_getv(vox, BlockScale);
    depth = depth > node_depth ? node_depth : depth;
    const int length = octree_size(depth);
    byte* grid = malloc(length * length * length);
    const CombineBones* table = zox_get(vox, CombineBones);
    const bone_parts b = {
        .grid = grid,
        .table = table,
        .depth = depth,
        .length = length,
        .size = block_scale * (1 << (node_depth - depth)),
        .bounds = calculate_vox_bounds(zox_getv(vox, ChunkSize), block_scale),
        .parents = parents,
        .bones = bones->length,
    };
    // the vox's octrees locked while read, the combine writes them (thread_safety.md)
    spinlock* lock = zox_octree_lock(world, vox);
    zox_octree_lock_begin(lock);
    bone_parts_fill(grid, length, table, zox_get(vox, BoneNode), 0, depth, int3_zero);
    zox_octree_lock_end(lock);
    for (int i = 0; i < verts->length; i += 4) {
        const byte bone = bone_parts_face(&b, &verts->value[i]);
        for (int j = 0; j < 4; j++) {
            out[i + j] = bone_parts_vertex(&b, verts->value[i + j], bone);
        }
    }
    free(grid);
    return 1;
}

// a skeleton mesh's weights from its vox's parts (all the chest's if it has none)
void generate_weights(ecs* world, entity e, entity vox, byte depth, const MeshVertices* verts, const BoneLinks* bones,
    BoneIndexes* weights)
{
    resize_BoneIndexes(weights, verts->length);
    if (disable_bones || !generate_weights_parts(world, vox, depth, verts, bones, weights->value)) {
        memset(weights->value, 0, sizeof(byte4) * weights->length);
    }
    zox_add(e, SkeletonMeshDirty);
}

// When the skeleton updates, regenerate weights for all its meshes
void bone_index_generate_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (!zox_has(mesh, SkeletonMesh) ||
                    zox_has(mesh, BuildMesh) ||
                    zox_has(mesh, BuildMeshWeights))
                {
                    continue;
                }
                zox_add(mesh, BuildMeshWeights);
            }
        }
        if (dbg_log) {
            zox_log("Skeleton Updated [%s], rebuilding weights", zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(bone_index_generate_system);

// Generates a skeleton mesh's weights from its parent's bones
void bone_index_generate_system2(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshVertices);
    zox_sys_out(BoneIndexes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshVertices, verts);
        zox_sys_o(BoneIndexes, weights);
        zox_remove(e, BuildMeshWeights);
        entity skeleton = zox_get_parent(world, e);
        if (!zox_valid(skeleton) || !zox_has(skeleton, BoneLinks)) {
            zox_loge("SkeletonMesh [%s] has no parent skeleton", zox_sys_e_name);
            continue;
        }
        const BoneLinks* bones = zox_get(skeleton, BoneLinks);
        generate_weights(world, e, get_skeleton_vox(world, skeleton), zox_getv(e, RenderDepth), verts, bones, weights);
        if (dbg_log) {
            zox_log("Weights Updated [%s] Bones [%i]",
                zox_sys_e_name,
                bones->length);
        }
    }
    zox_sys_on_end();
} zoxd_system(bone_index_generate_system2);
