// tst: body_poses, a character held in poses (arms forward, up, out, a stride, sitting...) to look for holes in its skin
//  - bash bsh/poses.sh (your settings, i.e. Block Res 5), then look at tst/body_poses.png (a row a pose, 4 views each:
//      +z, +x, -z, above), holes painted magenta
//  - the body_views setup (a character of the player's kind in the void, 4 viewport cameras), then its animations let go
//      of its bones (ShoulderBone, ThighBone, HeadBone tags off, the ShoulderBoneLink unlinked) and each pose is set on
//      its bones' LocalRotation3D every frame, a screenshot a pose: tst/body_poses/<n>_<pose>.ppm
//  - checks the skin is closed: each face's bone (its voxel's), every edge of a bone's faces met by an even count of its
//      faces and its joined bones' (parent, child): an odd one is open, a hole as the parts move apart (parts that only
//      touch had their faces between them culled, the whole body is one vox: an arm raised opened the chest's side)
//  - checks no joint cracks in each pose: the mesh's corners (4 vertices a face, none shared) moved as bone.vert moves
//      them, a corner's vertices on joined bones pulled apart (parts only touching part, each its own skin)
//  - the screenshots also get the background showing inside the body painted magenta (a patch the flood from a view's
//      edges can't reach): only a hint, a narrow gap between an arm and the chest is one too
//  - logs the bones (role, parent, position), the cracks and holes a pose
// tst: body_weights, the same poses with the skin painted by bone (paint_weights, bone.vert: a colour a bone, mixed at
//  joints by weight), to see which bone moves each part of the body and its clothes: tst/body_weights/<n>_<pose>.ppm
//  - bash bsh/poses.sh --weights, then tst/body_weights.png
//  - a clothes voxel's colour should be the body part's under it: a sleeve the arm's, the shorts' legs the thighs'
//  - the body_poses checks too: the clothes' parts crawling along them (combine_parts_resolve) left 242 open edges
#define zox_tst_poses_views 4
#define zox_tst_poses_settle 12     // frames a pose is held before its screenshot
#define zox_tst_poses_bones_max 64
// the views' background: green, nothing on a character is (body_views' gray is close to the shorts' shade)
const byte zox_tst_poses_background[3] = { 0, 160, 0 };

typedef enum {
    zox_pose_none, zox_pose_hips, zox_pose_head, zox_pose_shoulder_r, zox_pose_shoulder_l, zox_pose_arm_r, zox_pose_arm_l,
    zox_pose_forearm_r, zox_pose_forearm_l, zox_pose_hand_r, zox_pose_hand_l, zox_pose_thigh_r, zox_pose_thigh_l, zox_pose_shin_r, zox_pose_shin_l, zox_pose_roles
} zox_pose_role;
const char* zox_pose_role_names[zox_pose_roles] = { "-", "hips", "head", "shoulder_r", "shoulder_l", "arm_r", "arm_l",
    "forearm_r", "forearm_l", "hand_r", "hand_l", "thigh_r", "thigh_l", "shin_r", "shin_l" };

typedef struct {
    const char* name;
    float3 rotations[zox_pose_roles];   // euler degrees a role
} zox_tst_pose;

// right is -x (spawn_part_bones' is_right_side), x turns forward and back, z out to the side
const zox_tst_pose zox_tst_poses[] = {
    { "rest", { } },
    { "arms_forward", { [zox_pose_shoulder_r] = { -90, 0, 0 }, [zox_pose_shoulder_l] = { -90, 0, 0 } } },
    { "arms_up", { [zox_pose_shoulder_r] = { -170, 0, 0 }, [zox_pose_shoulder_l] = { -170, 0, 0 } } },
    { "arms_out", { [zox_pose_shoulder_r] = { 0, 0, -90 }, [zox_pose_shoulder_l] = { 0, 0, 90 } } },
    { "arms_back", { [zox_pose_shoulder_r] = { 60, 0, 0 }, [zox_pose_shoulder_l] = { 60, 0, 0 } } },
    { "elbows", { [zox_pose_shoulder_r] = { -45, 0, 0 }, [zox_pose_shoulder_l] = { -45, 0, 0 },
        [zox_pose_forearm_r] = { -90, 0, 0 }, [zox_pose_forearm_l] = { -90, 0, 0 } } },
    { "wrists", { [zox_pose_shoulder_r] = { -60, 0, 0 }, [zox_pose_shoulder_l] = { -60, 0, 0 },
        [zox_pose_hand_r] = { 0, 0, -60 }, [zox_pose_hand_l] = { 0, 0, 60 } } },
    { "stride", { [zox_pose_thigh_r] = { -45, 0, 0 }, [zox_pose_thigh_l] = { 45, 0, 0 },
        [zox_pose_shoulder_r] = { 40, 0, 0 }, [zox_pose_shoulder_l] = { -40, 0, 0 } } },
    { "sit", { [zox_pose_thigh_r] = { -90, 0, 0 }, [zox_pose_thigh_l] = { -90, 0, 0 },
        [zox_pose_shin_r] = { 90, 0, 0 }, [zox_pose_shin_l] = { 90, 0, 0 } } },
    { "legs_out", { [zox_pose_thigh_r] = { 0, 0, -40 }, [zox_pose_thigh_l] = { 0, 0, 40 } } },
    { "head", { [zox_pose_head] = { 25, 60, 0 } } },
    { "bend", { [zox_pose_hips] = { 0, 30, 20 }, [zox_pose_head] = { -30, -40, 0 } } },
};
#define zox_tst_poses_count ((int) (sizeof(zox_tst_poses) / sizeof(zox_tst_poses[0])))

byte zox_tst_poses_stage = 0;
int zox_tst_poses_index = 0;
int zox_tst_poses_frames = 0;
entity zox_tst_poses_bones[zox_tst_poses_bones_max];
byte zox_tst_poses_roles[zox_tst_poses_bones_max];
int zox_tst_poses_bones_length = 0;
char zox_tst_poses_files[16][512];
byte zox_tst_poses_painted = 0;     // body_weights: painted by bone, saved to tst/body_weights

// each bone's role from its tags (before they're taken off), the shins: a thigh's child that isn't one
static inline void zox_tst_poses_find(ecs* world, entity skeleton) {
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    zox_tst_poses_bones_length = 0;
    for (int i = 0; i < bones->length && i < zox_tst_poses_bones_max; i++) {
        const entity bone = bones->value[i];
        byte first = 1;
        for (int j = 0; j < zox_tst_poses_bones_length && first; j++) {
            first = zox_tst_poses_bones[j] != bone;
        }
        if (!first || !zox_valid(bone)) {
            continue;
        }
        const byte right = zox_getv(bone, BonePosition).x < 0;
        byte role = zox_pose_none;
        if (zox_has(bone, HipsBone)) {
            role = zox_pose_hips;
        } else if (zox_has(bone, HeadBone)) {
            role = zox_pose_head;
        } else if (zox_has(bone, ShoulderBone)) {
            role = right ? zox_pose_shoulder_r : zox_pose_shoulder_l;
        } else if (zox_has(bone, ArmBone) || zox_has(bone, HandBone)) {
            // down the arm: the upper arm (on the shoulder), the forearm, the hand
            const entity parent = zox_get_parent(world, bone);
            const entity grand = zox_valid(parent) ? zox_get_parent(world, parent) : 0;
            if (zox_valid(parent) && zox_has(parent, ShoulderBone)) {
                role = right ? zox_pose_arm_r : zox_pose_arm_l;
            } else if (zox_valid(grand) && zox_has(grand, ShoulderBone)) {
                role = right ? zox_pose_forearm_r : zox_pose_forearm_l;
            } else {
                role = right ? zox_pose_hand_r : zox_pose_hand_l;
            }
        } else if (zox_has(bone, ThighBone)) {
            role = right ? zox_pose_thigh_r : zox_pose_thigh_l;
        } else {
            const entity parent = zox_get_parent(world, bone);
            if (zox_valid(parent) && zox_has(parent, ThighBone)) {
                role = right ? zox_pose_shin_r : zox_pose_shin_l;
            }
        }
        const int index = zox_tst_poses_bones_length++;
        zox_tst_poses_bones[index] = bone;
        zox_tst_poses_roles[index] = role;
        const entity parent = zox_get_parent(world, bone);
        int parent_index = -1;
        for (int j = 0; j < index; j++) {
            if (zox_tst_poses_bones[j] == parent) {
                parent_index = j;
            }
        }
        const float3 p = zox_getv(bone, BonePosition), s = zox_getv(bone, BoneSize);
        zox_log("[tst] bone [%i] [%s] parent [%i] at [%.2f %.2f %.2f] size [%.2f %.2f %.2f]", index, zox_pose_role_names[role],
            parent_index, p.x, p.y, p.z, s.x, s.y, s.z);
    }
}

// the animations let go of the bones: the pose is the test's
static inline void zox_tst_poses_hold(ecs* world, entity skeleton) {
    for (int i = 0; i < zox_tst_poses_bones_length; i++) {
        const entity bone = zox_tst_poses_bones[i];
        if (zox_has(bone, ShoulderBone)) {
            zox_remove(bone, ShoulderBone);
        }
        if (zox_has(bone, ThighBone)) {
            zox_remove(bone, ThighBone);
        }
        if (zox_has(bone, HeadBone)) {
            zox_remove(bone, HeadBone);
        }
    }
    const entity shoulder = zox_get_link(world, skeleton, ShoulderBoneLink);
    if (zox_valid(shoulder)) {
        zox_unlink(world, skeleton, ShoulderBoneLink, shoulder);
    }
}

static inline void zox_tst_poses_set(ecs* world, const zox_tst_pose* pose) {
    for (int i = 0; i < zox_tst_poses_bones_length; i++) {
        const float3 euler = pose->rotations[zox_tst_poses_roles[i]];
        zox_setv(zox_tst_poses_bones[i], LocalRotation3D, quaternion_from_euler(float3_scale(euler, degrees_to_radians)));
    }
}

// the skin's cracks in its pose: each corner's vertices moved as bone.vert moves them, a corner pulled apart a crack
typedef struct {
    long long key;
    float3 p;
    byte bone;
    byte4 w;
} zox_tst_poses_corner;

static int zox_tst_poses_corner_compare(const void* x, const void* y) {
    const long long a = ((const zox_tst_poses_corner*) x)->key, b = ((const zox_tst_poses_corner*) y)->key;
    return a < b ? -1 : a > b;
}

static inline const char* zox_tst_poses_role_of(entity bone) {
    for (int i = 0; i < zox_tst_poses_bones_length; i++) {
        if (zox_tst_poses_bones[i] == bone) {
            return zox_pose_role_names[zox_tst_poses_roles[i]];
        }
    }
    return "?";
}

int zox_tst_poses_cracks[16];
float zox_tst_poses_gaps[16];
int zox_tst_poses_open = -1;

// the skin's open edges at rest: each face's bone (bone_parts_face), an edge's faces counted by bone, an edge is closed
//  for a bone if its faces and its joined bones' faces there are even
typedef struct {
    long long a, b;
    byte bone;
    float3 p, n;    // its middle, its face's normal
    byte axis;      // the axis it runs along
} zox_tst_poses_edge;

// a skeleton mesh's voxels' bones at depth (each face's bone), the grid malloc'd (free it), NULL if the vox has no parts
static inline byte* zox_tst_poses_parts_grid(ecs* world, entity vox, byte depth, int* length) {
    if (!zox_valid(vox) || !zox_has(vox, BoneNode) || !zox_has(vox, CombineBones) || !zox_get(vox, CombineBones)->length) {
        return NULL;
    }
    const byte node_depth = zox_getv(vox, NodeDepth);
    depth = depth > node_depth ? node_depth : depth;
    *length = octree_size(depth);
    byte* grid = malloc(*length * *length * *length);
    spinlock* lock = zox_octree_lock(world, vox);
    zox_octree_lock_begin(lock);
    bone_parts_fill(grid, *length, zox_get(vox, CombineBones), zox_get(vox, BoneNode), 0, depth, int3_zero);
    zox_octree_lock_end(lock);
    return grid;
}

static int zox_tst_poses_edge_compare(const void* x, const void* y) {
    const zox_tst_poses_edge* e = x;
    const zox_tst_poses_edge* f = y;
    return e->a != f->a ? (e->a < f->a ? -1 : 1) : e->b != f->b ? (e->b < f->b ? -1 : 1) : 0;
}

// a skeleton mesh's indices, vertices and weights: a gpu mesh's read back (bones/sys/skeleton_gpu.c), else its own
//  (copied), dispose them after
static inline void zox_tst_poses_mesh(ecs* world, entity mesh, MeshIndicies* indicies, MeshVertices* vertices,
    BoneIndexes* weights)
{
    if (zox_has(mesh, GpuMesh)) {
        MeshColors colors = { 0 };
        if (mesh3_read_gpu(world, mesh, indicies, vertices, &colors)) {
            resize_BoneIndexes(weights, vertices->length);
            zox_gpu_storage_read(zox_getv(mesh, BoneIndexGPULink), 0, sizeof(byte4) * vertices->length, weights->value);
        }
        dispose_MeshColors(&colors);
        return;
    }
    clone_MeshIndicies(indicies, zox_get(mesh, MeshIndicies));
    clone_MeshVertices(vertices, zox_get(mesh, MeshVertices));
    clone_BoneIndexes(weights, zox_get(mesh, BoneIndexes));
}

static inline void zox_tst_poses_open_edges(ecs* world, entity skeleton, entity mesh) {
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    MeshIndicies mesh_indicies = { 0 };
    MeshVertices mesh_vertices = { 0 };
    BoneIndexes mesh_weights = { 0 };
    zox_tst_poses_mesh(world, mesh, &mesh_indicies, &mesh_vertices, &mesh_weights);
    const MeshVertices* vertices = &mesh_vertices;
    const entity vox = get_skeleton_vox(world, skeleton);
    const byte depth = zox_getv(mesh, RenderDepth);
    int length = 0;
    byte* grid = zox_tst_poses_parts_grid(world, vox, depth, &length);
    if (!grid) {
        zox_tst_check(0, "the skeleton's vox has no parts");
        return;
    }
    byte parents[256];
    bone_parents_fill(world, bones, parents);
    // each part combined into the body, and its bone
    const CombineBones* table = zox_get(vox, CombineBones);
    const CombineList* list = zox_get(vox, CombineList);
    for (int k = 0; k < table->length && k < list->length; k++) {
        const byte bone = table->value[k];
        zox_log("[tst] part [%i] [%s] bone [%i] [%s]", k, zox_get_name(list->value[k]), bone,
            bone < bones->length ? zox_tst_poses_role_of(bones->value[bone]) : "?");
    }
    const byte node_depth = zox_getv(vox, NodeDepth);
    const float block_scale = zox_getv(vox, BlockScale);
    const byte d = depth > node_depth ? node_depth : depth;
    const bone_parts b = { .grid = grid, .table = zox_get(vox, CombineBones), .depth = d, .length = length,
        .size = block_scale * (1 << (node_depth - d)),
        .bounds = calculate_vox_bounds(zox_getv(vox, ChunkSize), block_scale), .parents = parents, .bones = bones->length };
    const float grid_size = block_scale * 0.5f;
    const int faces = vertices->length / 4;
    // a merged quad's sides (greedy meshing, chunks3/fun/greedy.c) are several voxels long: each side cut into its
    //  voxel long pieces, so they meet the pieces of the faces beside it
    int edges_count = 0, edges_room = faces * 4;
    zox_tst_poses_edge* edges = malloc(sizeof(zox_tst_poses_edge) * edges_room);
    for (int f = 0; f < faces; f++) {
        const float3* v = &vertices->value[f * 4];
        const int* face = &mesh_indicies.value[f * 6];
        const int tri[3] = { face[0] - f * 4, face[1] - f * 4, face[2] - f * 4 };
        const byte bone = bone_parts_face(&b, v, tri);
        // a quad's 4 sides: of its 6 corner pairs, the 2 longest are its diagonals
        const int pairs[6][2] = { { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 2 }, { 1, 3 }, { 2, 3 } };
        float lengths[6];
        for (int k = 0; k < 6; k++) {
            lengths[k] = float3_length(float3_sub(v[pairs[k][0]], v[pairs[k][1]]));
        }
        int diagonal_a = 0, diagonal_b = -1;
        for (int k = 1; k < 6; k++) {
            if (lengths[k] > lengths[diagonal_a]) {
                diagonal_a = k;
            }
        }
        for (int k = 0; k < 6; k++) {
            if (k != diagonal_a && (diagonal_b < 0 || lengths[k] > lengths[diagonal_b])) {
                diagonal_b = k;
            }
        }
        for (int k = 0; k < 6; k++) {
            if (k == diagonal_a || k == diagonal_b) {
                continue;
            }
            const float3 from = v[pairs[k][0]], to = v[pairs[k][1]];
            const float3 along = float3_sub(from, to);
            const byte axis = fabsf(along.x) > fabsf(along.y) ? (fabsf(along.x) > fabsf(along.z) ? 0 : 2) :
                (fabsf(along.y) > fabsf(along.z) ? 1 : 2);
            const int pieces = int_max(1, (int) lroundf(float3_length(along) / b.size));
            for (int piece = 0; piece < pieces; piece++) {
                const float3 p0 = float3_add(to, float3_scale(along, (float) piece / pieces));
                const float3 p1 = float3_add(to, float3_scale(along, (float) (piece + 1) / pieces));
                const long long x = (((long long) lroundf(p0.x / grid_size) + 4096) << 40) |
                    (((long long) lroundf(p0.y / grid_size) + 4096) << 20) | ((long long) lroundf(p0.z / grid_size) + 4096);
                const long long y = (((long long) lroundf(p1.x / grid_size) + 4096) << 40) |
                    (((long long) lroundf(p1.y / grid_size) + 4096) << 20) | ((long long) lroundf(p1.z / grid_size) + 4096);
                if (edges_count == edges_room) {
                    edges_room *= 2;
                    edges = realloc(edges, sizeof(zox_tst_poses_edge) * edges_room);
                }
                edges[edges_count++] = (zox_tst_poses_edge) { x < y ? x : y, x < y ? y : x, bone, .axis = axis,
                    .p = float3_scale(float3_add(p0, p1), 0.5f),
                    .n = float3_normalize(float3_cross(float3_sub(v[tri[1]], v[tri[0]]), float3_sub(v[tri[2]], v[tri[0]]))) };
            }
        }
    }
    qsort(edges, edges_count, sizeof(zox_tst_poses_edge), zox_tst_poses_edge_compare);
    int open = 0, logged = 0;
    for (int i = 0; i < edges_count;) {
        int j = i;
        int counts[256] = { 0 };
        while (j < edges_count && edges[j].a == edges[i].a && edges[j].b == edges[i].b) {
            counts[edges[j].bone]++;
            j++;
        }
        if (edges[i].a >= 0) {
            // the bones of the 4 voxels round it, joined ones (parent, child, through each other) one piece of skin, as
            //  the weights join them (bones/sys/generate.c): a chest and arm face with the shoulder between blend
            byte around[4];
            const float h = b.size * 0.5f;
            for (int c = 0; c < 4; c++) {
                float3 at = edges[i].p;
                const float u = c & 1 ? h : -h, w = c & 2 ? h : -h;
                if (edges[i].axis == 0) {
                    at.y += u;
                    at.z += w;
                } else if (edges[i].axis == 1) {
                    at.x += u;
                    at.z += w;
                } else {
                    at.x += u;
                    at.y += w;
                }
                around[c] = bone_parts_at(&b, bone_parts_grid(&b, at));
            }
            byte piece[256];
            for (int n = 0; n < 256; n++) {
                piece[n] = n;
            }
            for (byte grew = 1; grew;) {
                grew = 0;
                for (int c = 0; c < 4; c++)
                for (int d = 0; d < 4; d++) {
                    const byte x = around[c], y = around[d];
                    if (x != 255 && y != 255 && piece[x] != piece[y] && bone_parts_joined(&b, x, y)) {
                        const byte low = piece[x] < piece[y] ? piece[x] : piece[y];
                        piece[x] = piece[y] = low;
                        grew = 1;
                    }
                }
            }
            for (int k = i; k < j; k++) {
                const byte bone = edges[k].bone;
                int sum = 0;
                for (int n = 0; n < 256; n++) {
                    sum += counts[n] && (n == bone || piece[n] == piece[bone]) ? counts[n] : 0;
                }
                if (sum % 2) {
                    open++;
                    if (logged++ < 12) {
                        char line[160] = "";
                        for (int n = 0; n < 256; n++) {
                            if (counts[n]) {
                                char part[32];
                                snprintf(part, sizeof(part), " [%s %i]x%i", n < bones->length ?
                                    zox_tst_poses_role_of(bones->value[n]) : "?", n, counts[n]);
                                strncat(line, part, sizeof(line) - strlen(line) - 1);
                            }
                        }
                        const float3 at = edges[i].p;
                        zox_log("[tst]   open edge of [%s %i] at [%.3f %.3f %.3f]:%s", zox_tst_poses_role_of(bones->value[bone]), bone,
                            at.x, at.y, at.z, line);
                        for (int q = i; q < j; q++) {
                            zox_log("[tst]     face [%s %i] normal [%.0f %.0f %.0f]", zox_tst_poses_role_of(bones->value[edges[q].bone]),
                                edges[q].bone, edges[q].n.x, edges[q].n.y, edges[q].n.z);
                        }
                    }
                    break;
                }
            }
        }
        i = j;
    }
    free(edges);
    free(grid);
    dispose_MeshIndicies(&mesh_indicies);
    dispose_MeshVertices(&mesh_vertices);
    dispose_BoneIndexes(&mesh_weights);
    zox_tst_poses_open = open;
    zox_log("[tst] the skin at rest: [%i] faces, [%i] open edges", faces, open);
}

static inline void zox_tst_poses_measure(ecs* world, entity skeleton, int pose) {
    const entity mesh = zox_get_link(world, skeleton, ActiveMesh);
    if (!zox_valid(mesh) || !zox_has(mesh, BoneIndexes)) {
        zox_tst_check(0, "no weighted skeleton mesh for pose [%s]", zox_tst_poses[pose].name);
        return;
    }
    if (!pose) {
        zox_tst_poses_open_edges(world, skeleton, mesh);
    }
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    MeshIndicies mesh_indicies = { 0 };
    MeshVertices mesh_vertices = { 0 };
    BoneIndexes mesh_weights = { 0 };
    zox_tst_poses_mesh(world, mesh, &mesh_indicies, &mesh_vertices, &mesh_weights);
    const MeshVertices* vertices = &mesh_vertices;
    const BoneIndexes* weights = &mesh_weights;
    const int length = vertices->length < weights->length ? vertices->length : weights->length;
    float4x4 matrices[zox_bones_max];
    float3 positions[zox_bones_max];
    for (int j = 0; j < bones->length && j < zox_bones_max; j++) {
        matrices[j] = zox_getv(bones->value[j], TransformMatrix);
        positions[j] = zox_getv(bones->value[j], BonePosition);
    }
    const entity vox = get_skeleton_vox(world, skeleton);
    const float voxel = zox_getv(vox, BlockScale) * (1 << (zox_getv(vox, NodeDepth) - zox_getv(mesh, RenderDepth)));
    const float grid = zox_getv(vox, BlockScale) * 0.5f;
    zox_tst_poses_corner* corners = malloc(sizeof(zox_tst_poses_corner) * length);
    for (int i = 0; i < length; i++) {
        const float3 v = vertices->value[i];
        const byte4 w = weights->value[i];
        const byte a = w.x < bones->length ? w.x : 0, b = w.y < bones->length ? w.y : 0;
        const float3 pa = float4x4_multiply_float3(matrices[a], float3_sub(v, positions[a]));
        const float3 pb = float4x4_multiply_float3(matrices[b], float3_sub(v, positions[b]));
        const float t = w.z / 255.0f;
        corners[i] = (zox_tst_poses_corner) {
            .key = (((long long) lroundf(v.x / grid) + 4096) << 40) | (((long long) lroundf(v.y / grid) + 4096) << 20) |
                ((long long) lroundf(v.z / grid) + 4096),
            .p = float3_add(float3_scale(pa, 1 - t), float3_scale(pb, t)),
            .bone = t > 0.5f ? b : a, .w = w };
    }
    qsort(corners, length, sizeof(zox_tst_poses_corner), zox_tst_poses_corner_compare);
    // a crack: wider than a tenth of a voxel; counted by the bones either side
    typedef struct { byte a, b; int count; float gap; float3 at; } zox_tst_poses_pair;
    zox_tst_poses_pair pairs[64];
    int pairs_length = 0, cracks = 0;
    float widest = 0;
    for (int i = 0; i < length;) {
        int j = i + 1;
        float gap = 0;
        byte other = corners[i].bone;
        for (; j < length && corners[j].key == corners[i].key; j++) {
            const float d = float3_length(float3_sub(corners[j].p, corners[i].p));
            if (d > gap) {
                gap = d;
                other = corners[j].bone;
            }
        }
        const entity ea = bones->value[corners[i].bone], eb = bones->value[other];
        const byte at_joint = ea == eb || zox_get_parent(world, ea) == eb || zox_get_parent(world, eb) == ea;
        if (gap > voxel * 0.1f) {
            if (at_joint && cracks < 3) {
                for (int k = i; k < j; k++) {
                    zox_log("[tst]     joint corner vertex weights [%i %i %i]", corners[k].w.x, corners[k].w.y, corners[k].w.z);
                }
            }
            cracks += at_joint;
            widest = gap > widest ? gap : widest;
            const byte a = corners[i].bone < other ? corners[i].bone : other, b = corners[i].bone < other ? other : corners[i].bone;
            int k = 0;
            while (k < pairs_length && (pairs[k].a != a || pairs[k].b != b)) {
                k++;
            }
            if (k == pairs_length && pairs_length < 64) {
                pairs[pairs_length++] = (zox_tst_poses_pair) { a, b, 0, 0, corners[i].p };
            }
            if (k < pairs_length) {
                pairs[k].count++;
                if (gap > pairs[k].gap) {
                    pairs[k].gap = gap;
                }
            }
        }
        i = j;
    }
    free(corners);
    dispose_MeshIndicies(&mesh_indicies);
    dispose_MeshVertices(&mesh_vertices);
    dispose_BoneIndexes(&mesh_weights);
    zox_tst_poses_cracks[pose] = cracks;
    zox_tst_poses_gaps[pose] = widest / voxel;
    zox_log("[tst] pose [%s] joint cracks [%i], parts apart widest [%.1f] voxels", zox_tst_poses[pose].name, cracks, widest / voxel);
    for (int k = 0; k < pairs_length; k++) {
        const entity a = bones->value[pairs[k].a], b = bones->value[pairs[k].b];
        const byte joined = a == b || zox_get_parent(world, a) == b || zox_get_parent(world, b) == a;
        zox_log("[tst]   [%i] corners between [%s %i] and [%s %i] (%s), widest [%.1f] voxels", pairs[k].count,
            zox_tst_poses_role_of(a), pairs[k].a, zox_tst_poses_role_of(b), pairs[k].b, joined ? "a joint" : "only touching",
            pairs[k].gap / voxel);
    }
}

byte zox_tst_body_poses_ready(ecs* world) {
    // the body_views setup, up to its cameras
    if (zox_tst_body_views_stage < 3) {
        zox_tst_body_views_ready(world);
        return 0;
    }
    const entity character = zox_tst_body_views_character;
    if (!zox_valid(character)) {
        zox_tst_check(0, "the character is gone");
        return 1;
    }
    zox_setv(character, Position3D, zox_tst_body_views_spot);
    zox_setv(character, Rotation3D, quaternion_identity);
    if (zox_has(character, Velocity3D)) {
        zox_setv(character, Velocity3D, float3_zero);
    }
    const entity skeleton = get_skeleton(world, character);
    // a body rebuild (its gear landing late) spawns new bones: found again, the pose done again
    for (int i = 0; i < zox_tst_poses_bones_length && zox_tst_poses_stage; i++) {
        if (!zox_valid(zox_tst_poses_bones[i])) {
            zox_log("[tst] the bones were rebuilt, finding them again");
            zox_tst_poses_stage = 0;
            zox_tst_poses_frames = 0;
        }
    }
    for (int v = 0; v < zox_tst_poses_views; v++) {
        const entity camera = zox_tst_body_views_cameras[v];
        if (zox_valid(camera)) {
            zox_setv(camera, Color, ((color) { zox_tst_poses_background[0], zox_tst_poses_background[1],
                zox_tst_poses_background[2], 255 }));
        }
    }
    if (!zox_tst_poses_stage) {
        const entity active = zox_valid(skeleton) ? zox_get_link(world, skeleton, ActiveMesh) : 0;
        if (zox_getv(character, BodyDirty) != zox_generate_body_end || !zox_valid(active) ||
            zox_has(active, SkeletonMeshStale) || !is_skeleton_mesh_ready(world, active) ||
            zox_valid(zox_get_link(world, skeleton, PreparingMesh)))
        {
            zox_tst_waiting = "the body to settle";
            zox_tst_progress(zox_current_time);
            return 0;
        }
        zox_tst_poses_find(world, skeleton);
        zox_tst_poses_hold(world, skeleton);
        zox_tst_poses_stage = 1;
        zox_tst_poses_frames = 0;
    }
    zox_tst_progress(zox_tst_poses_index * 1000 + zox_tst_poses_frames);
    zox_tst_waiting = "the poses' screenshots";
    if (zox_tst_poses_index >= zox_tst_poses_count) {
        return !zox_screenshot_request[0];
    }
    const zox_tst_pose* pose = &zox_tst_poses[zox_tst_poses_index];
    zox_tst_poses_set(world, pose);
    zox_tst_poses_frames++;
    if (zox_tst_poses_frames == zox_tst_poses_settle - 1) {
        zox_tst_poses_measure(world, skeleton, zox_tst_poses_index);
    }
    if (zox_tst_poses_frames == zox_tst_poses_settle) {
        char name[128];
        snprintf(name, sizeof(name), "%s/%02i_%s.ppm", zox_tst_poses_painted ? "body_weights" : "body_poses",
            zox_tst_poses_index, pose->name);
        snprintf(zox_tst_poses_files[zox_tst_poses_index], sizeof(zox_tst_poses_files[0]), "%s", zox_tst_path(name));
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%.255s", zox_tst_poses_files[zox_tst_poses_index]);
    } else if (zox_tst_poses_frames > zox_tst_poses_settle && !zox_screenshot_request[0]) {
        zox_tst_poses_index++;
        zox_tst_poses_frames = 0;
    }
    return 0;
}

static inline void zox_tst_poses_save_ppm(const char* path, const byte* pixels, int2 size) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        return;
    }
    fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
    fwrite(pixels, 1, (size_t) size.x * size.y * 3, file);
    fclose(file);
}

// the background patches inside a view the flood from its edges can't reach, painted magenta
static inline int zox_tst_poses_holes(byte* pixels, int2 size, int2 corner, int2 view, byte* reached, int* stack) {
    #define zox_tst_poses_gray(x, y) (zox_tst_pixel_difference(zox_tst_ppm_pixel(pixels, size, x, y), zox_tst_poses_background) <= 24)
    memset(reached, 0, (size_t) view.x * view.y);
    int top = 0;
    for (int x = 0; x < view.x; x++) {
        for (int k = 0; k < 2; k++) {
            const int y = k ? view.y - 1 : 0;
            if (zox_tst_poses_gray(corner.x + x, corner.y + y) && !reached[y * view.x + x]) {
                reached[y * view.x + x] = 1;
                stack[top++] = y * view.x + x;
            }
        }
    }
    for (int y = 0; y < view.y; y++) {
        for (int k = 0; k < 2; k++) {
            const int x = k ? view.x - 1 : 0;
            if (zox_tst_poses_gray(corner.x + x, corner.y + y) && !reached[y * view.x + x]) {
                reached[y * view.x + x] = 1;
                stack[top++] = y * view.x + x;
            }
        }
    }
    while (top) {
        const int at = stack[--top];
        const int x = at % view.x, y = at / view.x;
        const int2 next[4] = { { x - 1, y }, { x + 1, y }, { x, y - 1 }, { x, y + 1 } };
        for (int k = 0; k < 4; k++) {
            const int2 n = next[k];
            if (n.x < 0 || n.y < 0 || n.x >= view.x || n.y >= view.y || reached[n.y * view.x + n.x]) {
                continue;
            }
            if (zox_tst_poses_gray(corner.x + n.x, corner.y + n.y)) {
                reached[n.y * view.x + n.x] = 1;
                stack[top++] = n.y * view.x + n.x;
            }
        }
    }
    int holes = 0;
    for (int y = 0; y < view.y; y++) {
        for (int x = 0; x < view.x; x++) {
            if (!reached[y * view.x + x] && zox_tst_poses_gray(corner.x + x, corner.y + y)) {
                holes++;
                byte* p = (byte*) zox_tst_ppm_pixel(pixels, size, corner.x + x, corner.y + y);
                p[0] = 255;
                p[1] = 0;
                p[2] = 255;
            }
        }
    }
    #undef zox_tst_poses_gray
    return holes;
}

void zox_tst_body_poses_check(ecs* world) {
    int total = 0, posed_with_holes = 0;
    for (int i = 0; i < zox_tst_poses_count; i++) {
        int2 size;
        byte* pixels = zox_tst_load_ppm(zox_tst_poses_files[i], &size);
        zox_tst_check(pixels, "no screenshot for pose [%s]", zox_tst_poses[i].name);
        if (!pixels) {
            continue;
        }
        const int2 view = { size.x / 2, size.y / 2 };
        byte* reached = malloc((size_t) view.x * view.y);
        int* stack = malloc(sizeof(int) * view.x * view.y);
        int holes[zox_tst_poses_views];
        int pose_holes = 0;
        for (int v = 0; v < zox_tst_poses_views; v++) {
            const int2 corner = { (v % 2) * view.x, (v / 2) * view.y };
            holes[v] = zox_tst_poses_holes(pixels, size, corner, view, reached, stack);
            pose_holes += holes[v];
        }
        zox_tst_poses_save_ppm(zox_tst_poses_files[i], pixels, size);
        free(stack);
        free(reached);
        free(pixels);
        zox_log("[tst] pose [%s] joint cracks [%i] parts apart [%.1f] voxels, background inside [%i] (+z [%i] +x [%i] -z [%i] above [%i])", zox_tst_poses[i].name,
            zox_tst_poses_cracks[i], zox_tst_poses_gaps[i], pose_holes, holes[0], holes[1], holes[2], holes[3]);
        total += zox_tst_poses_cracks[i];
        posed_with_holes += zox_tst_poses_cracks[i] > 0;
    }
    zox_log("[tst] [metric] body_poses [%i] open edges at rest, [%i] joint cracks, [%i] of [%i] poses crack",
        zox_tst_poses_open, total, posed_with_holes, zox_tst_poses_count);
    zox_tst_check(!zox_tst_poses_open, "the skin has [%i] open edges (holes as its parts move): see tst/body_poses.png",
        zox_tst_poses_open);
    zox_tst_check(!posed_with_holes, "[%i] of [%i] poses crack at a joint ([%i] corners): see tst/body_poses.png",
        posed_with_holes, zox_tst_poses_count, total);
}

byte zox_tst_body_weights_ready(ecs* world) {
    zox_tst_poses_painted = 1;
    paint_weights = 1;
    return zox_tst_body_poses_ready(world);
}

void zox_tsts_body_poses(ecs* world) {
    zox_tst_add("body_poses", zox_tst_body_poses_ready, zox_tst_body_poses_check, 240);
    zox_tst_set_setup("body_poses", NULL, 1);
    zox_tst_add("body_weights", zox_tst_body_weights_ready, zox_tst_body_poses_check, 240);
    zox_tst_set_setup("body_weights", NULL, 1);
}
