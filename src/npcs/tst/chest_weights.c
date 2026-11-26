// tst: chest_weights, a humanoid's mesh moves with its voxels' bones (checks below)
//  - spawns a humanoid by the player, waits for its body, checks its active skeleton mesh's weights
//  - run: bash bsh/tst.sh chest_weights
int zox_tst_cweights_frames = 0;
entity zox_tst_cweights_npc = 0;

byte zox_tst_chest_weights_ready(ecs* world) {
    entity realm, terrain;
    if (!zox_tst_npc_world(world, &realm, &terrain)) {
        return 0;
    }
    zox_tst_cweights_frames++;
    zox_tst_progress(zox_tst_cweights_frames);
    zox_tst_waiting = "a humanoid's body";
    if (zox_tst_cweights_frames == 10) {
        zox_tst_cweights_npc = spawn_character3_npc(world, zox_tst_realm_humanoid(world, realm), realm,
            terrain, 1, 0, 0, zox_tst_npc_void, quaternion_identity, NULL);
    }
    return zox_tst_cweights_frames > 150;
}

// a humanoid's skeleton mesh moves with its voxels' parts (bones/sys/generate.c, doc/done/skeletons.md)
//  - every solid voxel of its vox has a part (BoneNode), none left waiting
//  - a face's 4 vertices share its bone; blends only between a bone and its parent or child, and some joints blend
//  - its arms beside the chest never move with it (an arm's front strip stayed still, the sleeve tore)
//  - the box weights it replaced gave 1153 of 18268 vertices another bone at every joint (chest 149 hips, 146 a
//    shoulder; hips 214 a thigh; head 86 the chest; shoulder 78 the arm)
// a vertex's place and its bones in one order (the lower bone first, the other's weight), to compare corners
typedef struct {
    long long key;
    byte a, b, weight;
    float3 p;
} zox_tst_cweights_corner;

static int zox_tst_cweights_compare(const void* x, const void* y) {
    const long long a = ((const zox_tst_cweights_corner*) x)->key, b = ((const zox_tst_cweights_corner*) y)->key;
    return a < b ? -1 : a > b;
}

void zox_tst_chest_weights_check(ecs* world) {
    const entity skeleton = get_skeleton(world, zox_tst_cweights_npc);
    const entity mesh = zox_valid(skeleton) ? zox_get_link(world, skeleton, ActiveMesh) : 0;
    zox_tst_check(zox_valid(mesh) && zox_has(mesh, BoneIndexes), "the humanoid has no weighted skeleton mesh");
    if (!zox_valid(mesh) || !zox_has(mesh, BoneIndexes)) {
        return;
    }
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    const MeshVertices* vertices = zox_get(mesh, MeshVertices);
    const BoneIndexes* weights = zox_get(mesh, BoneIndexes);
    // meshed and weighted on the gpu (bones/sys/skeleton_gpu.c): its vertices and weights read back
    MeshIndicies gpu_indicies = { 0 };
    MeshVertices gpu_vertices = { 0 };
    MeshColors gpu_colors = { 0 };
    BoneIndexes gpu_weights = { 0 };
    const byte gpu = zox_has(mesh, GpuMesh) && mesh3_read_gpu(world, mesh, &gpu_indicies, &gpu_vertices, &gpu_colors);
    if (gpu) {
        resize_BoneIndexes(&gpu_weights, gpu_vertices.length);
        zox_gpu_storage_read(zox_getv(mesh, BoneIndexGPULink), 0, sizeof(byte4) * gpu_vertices.length, gpu_weights.value);
        vertices = &gpu_vertices;
        weights = &gpu_weights;
    }
    zox_log("[tst] chest_weights: [%i] vertices, meshed on the [%s]", vertices->length, gpu ? "gpu" : "cpu");
    const entity vox = get_skeleton_vox(world, skeleton);
    zox_tst_check(zox_valid(vox) && zox_has(vox, BoneNode), "the humanoid's vox has no BoneNode");
    if (!zox_valid(vox) || !zox_has(vox, BoneNode)) {
        return;
    }
    const byte depth = zox_getv(vox, NodeDepth);
    const int length = octree_size(depth);
    const VoxelNode* voxels = zox_get(vox, VoxelNode);
    const BoneNode* parts = zox_get(vox, BoneNode);
    int solid = 0, unparted = 0, waiting = 0;
    for (int x = 0; x < length; x++)
    for (int y = 0; y < length; y++)
    for (int z = 0; z < length; z++) {
        const byte3 p = { x, y, z };
        if (getv_VoxelNode(voxels, depth, p)) {
            const byte part = getv_BoneNode(parts, depth, p);
            solid++;
            unparted += !part;
            waiting += part == 255;
        }
    }
    int split_faces = 0, blended = 0, unjoined = 0, beside = 0, beside_chest = 0;
    const float3 cc = zox_getv(bones->value[0], BonePosition), cs = zox_getv(bones->value[0], BoneSize);
    for (int i = 0; i < vertices->length && i < weights->length; i++) {
        const byte4 w = weights->value[i];
        split_faces += i % 4 == 0 && i + 3 < weights->length &&
            (weights->value[i + 1].x != w.x || weights->value[i + 2].x != w.x || weights->value[i + 3].x != w.x);
        if (w.z) {
            blended++;
            const entity a = bones->value[w.x], b = bones->value[w.y];
            unjoined += zox_get_parent(world, a) != b && zox_get_parent(world, b) != a;
        }
        // beside the chest (an arm, a sleeve), past its side and level with it
        const float3 p = vertices->value[i];
        if (fabsf(p.x - cc.x) > cs.x * 1.2f && fabsf(p.y - cc.y) < cs.y) {
            beside++;
            beside_chest += w.x == 0 || (w.z && w.y == 0);
        }
    }
    // cracks: faces sharing a corner move it differently, the skin opens there as the bones turn
    //  - at a joint (their bones joined, or the same): the blend disagrees; else a seam between parts that only touch
    zox_tst_cweights_corner* corners = malloc(sizeof(zox_tst_cweights_corner) * vertices->length);
    const float grid = zox_getv(vox, BlockScale) * 0.5f;
    for (int i = 0; i < vertices->length; i++) {
        const float3 p = vertices->value[i];
        const byte4 w = weights->value[i];
        const byte blend = w.z && w.x != w.y;
        corners[i] = (zox_tst_cweights_corner) {
            .key = (((long long) lroundf(p.x / grid) + 4096) << 40) | (((long long) lroundf(p.y / grid) + 4096) << 20) |
                ((long long) lroundf(p.z / grid) + 4096),
            .a = !blend || w.x < w.y ? w.x : w.y,
            .b = !blend ? w.x : w.x < w.y ? w.y : w.x,
            .weight = !blend ? 0 : w.x < w.y ? w.z : 255 - w.z, .p = p };
    }
    qsort(corners, vertices->length, sizeof(zox_tst_cweights_corner), zox_tst_cweights_compare);
    int joint_cracks = 0, seams = 0, places = 0;
    for (int i = 0; i < vertices->length;) {
        int j = i + 1;
        byte cracked = 0, joint = 0;
        for (; j < vertices->length && corners[j].key == corners[i].key; j++) {
            const zox_tst_cweights_corner c = corners[i], d = corners[j];
            if (c.a == d.a && c.b == d.b && abs(c.weight - d.weight) <= 2) {
                continue;
            }
            if (!cracked) {
                zox_log("[tst] crack: bones [%i %i weight %i] and [%i %i weight %i] at [%.2f %.2f %.2f]", c.a, c.b, c.weight, d.a, d.b,
                    d.weight, c.p.x, c.p.y, c.p.z);
            }
            cracked = 1;
            const entity ea = bones->value[c.weight > 127 ? c.b : c.a], eb = bones->value[d.weight > 127 ? d.b : d.a];
            joint |= ea == eb || zox_get_parent(world, ea) == eb || zox_get_parent(world, eb) == ea;
        }
        places++;
        joint_cracks += cracked && joint;
        seams += cracked && !joint;
        i = j;
    }
    free(corners);
    zox_log("[tst] [metric] chest_weights [%i] corners, [%i] crack at a joint, [%i] are seams between parts that only touch", places, joint_cracks, seams);
    zox_tst_check(!joint_cracks, "[%i] corners crack at a joint: their faces blend them differently", joint_cracks);
    zox_log("[tst] [metric] chest_weights [%i] bones, voxels [%i] without a part [%i] waiting [%i]", bones->length, solid, unparted, waiting);
    zox_log("[tst] [metric] chest_weights [%i] vertices, [%i] blended at joints, [%i] beside the chest", vertices->length, blended, beside);
    zox_tst_check(solid && !unparted, "[%i] of [%i] voxels have no part", unparted, solid);
    zox_tst_check(!waiting, "[%i] clothes voxels never found the part under them", waiting);
    // (a merged face reaching past its bone's voxels takes each corner's own bones, so its vertices can differ)
    zox_log("[tst] [metric] chest_weights [%i] faces have vertices on different bones", split_faces);
    zox_tst_check(blended, "no joints blend");
    zox_tst_check(!unjoined, "[%i] vertices blend between bones that aren't joined", unjoined);
    zox_tst_check(beside > 0 && !beside_chest, "[%i] of [%i] vertices beside the chest move with it", beside_chest, beside);
    if (gpu) {
        dispose_MeshIndicies(&gpu_indicies);
        dispose_MeshVertices(&gpu_vertices);
        dispose_MeshColors(&gpu_colors);
        dispose_BoneIndexes(&gpu_weights);
    }
}

void zox_tsts_chest_weights(ecs* world) {
    zox_tst_add("chest_weights", zox_tst_chest_weights_ready, zox_tst_chest_weights_check, 120);
    zox_tst_set_setup("chest_weights", NULL, 1);
}
