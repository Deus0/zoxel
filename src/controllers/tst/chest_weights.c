// tst: chest_weights, a humanoid's mesh moves with its voxels' bones (checks below)
//  - spawns a humanoid by the player, waits for its body, checks its active skeleton mesh's weights
//  - run: bash bsh/tst.sh chest_weights
int zox_tst_cweights_frames = 0;
entity zox_tst_cweights_npc = 0;

byte zox_tst_chest_weights_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character) || zox_getv(player, PlayerState) != zox_player_state_playing) {
        zox_tst_waiting = "playing";
        return 0;
    }
    zox_tst_cweights_frames++;
    zox_tst_progress(zox_tst_cweights_frames);
    zox_tst_waiting = "a humanoid's body";
    if (zox_tst_cweights_frames == 10) {
        const entity realm = zox_get_link(world, character, RealmLink);
        float3 position = zox_getv(character, Position3D);
        position.x += 2;
        zox_tst_cweights_npc = spawn_character3_npc(world, zox_tst_realm_humanoid(world, realm), realm,
            zox_get_link(world, character, TerrainLink), 1, 0, 0, position, quaternion_identity, NULL);
    }
    return zox_tst_cweights_frames > 150;
}

// a humanoid's skeleton mesh moves with its voxels' parts (bones/sys/generate.c, doc/plans/skeletons.md)
//  - every solid voxel of its vox has a part (BoneNode), none left waiting
//  - a face's 4 vertices share its bone; blends only between a bone and its parent or child, and some joints blend
//  - its arms beside the chest never move with it (an arm's front strip stayed still, the sleeve tore)
//  - the box weights it replaced gave 1153 of 18268 vertices another bone at every joint (chest 149 hips, 146 a
//    shoulder; hips 214 a thigh; head 86 the chest; shoulder 78 the arm)
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
    zox_log("[tst] [metric] chest_weights [%i] bones, voxels [%i] without a part [%i] waiting [%i]", bones->length, solid, unparted, waiting);
    zox_log("[tst] [metric] chest_weights [%i] vertices, [%i] blended at joints, [%i] beside the chest", vertices->length, blended, beside);
    zox_tst_check(solid && !unparted, "[%i] of [%i] voxels have no part", unparted, solid);
    zox_tst_check(!waiting, "[%i] clothes voxels never found the part under them", waiting);
    zox_tst_check(!split_faces, "[%i] faces have vertices on different bones", split_faces);
    zox_tst_check(blended, "no joints blend");
    zox_tst_check(!unjoined, "[%i] vertices blend between bones that aren't joined", unjoined);
    zox_tst_check(beside > 0 && !beside_chest, "[%i] of [%i] vertices beside the chest move with it", beside_chest, beside);
}
