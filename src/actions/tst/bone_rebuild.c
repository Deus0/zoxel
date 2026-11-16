// tst: bone_rebuild, the hat taken off then put back on (its bone gone then back) with a cookie held: nothing flickers
//  - a bone keeps its index over a rebuild (the old skeleton mesh draws until the new one swaps in, its weights are by
//      index): every bone still there is at its index from before
//  - the drawn skeleton mesh never changes under it (rebuilt in place, or reweighted for new voxels)
//  - the held item stays held, the camera keeps its look on the head, no bone jumps (from the character)
//  - logs a line a frame: the bones, the skeleton meshes (drawn or not)
//  - run: bash bsh/tst.sh bone_rebuild
#define zox_tst_rebuild_off_frames 60
#define zox_tst_rebuild_frames 130
byte zox_tst_rebuild_stage = 0;
int zox_tst_rebuild_frame = 0;
entity zox_tst_rebuild_hat_slot = 0;
entity zox_tst_rebuild_hat = 0;
int zox_tst_rebuild_hat_bones = 0;     // the bone count with the hat on
byte zox_tst_rebuild_off_seen = 0;     // a frame drew the bones without the hat's
entity zox_tst_rebuild_slots[64];      // each index's slot the frame before
int zox_tst_rebuild_slots_length = 0;
int zox_tst_rebuild_moved = 0;         // frames a bone still there moved index
int zox_tst_rebuild_undrawn = 0;       // frames the skeleton had no mesh to draw
int zox_tst_rebuild_item_gone = 0;     // frames the held item was missing
int zox_tst_rebuild_jumps = 0;         // frames a bone's transform jumped
int zox_tst_rebuild_unready = 0;       // frames a drawn skeleton mesh changed under it
int zox_tst_rebuild_look_lost = 0;     // frames the camera wasn't on the head with the look it had
int zox_tst_rebuild_meshes = 0;        // skeleton meshes swapped in
entity zox_tst_rebuild_drawn_mesh = 0;
int zox_tst_rebuild_drawn_length = 0;
float4 zox_tst_rebuild_look;
float4x4 zox_tst_rebuild_last[64];
int zox_tst_rebuild_last_length = 0;

// how far a bone moved from the last frame, from the character (a new bone drawn at identity is far off, a leg
//  swinging at a test's low frame rate isn't)
static inline float zox_tst_rebuild_difference(float4x4 a, float4x4 b) {
    const float* x = (const float*) &a;
    const float* y = (const float*) &b;
    return fabsf(x[12] - y[12]) + fabsf(x[13] - y[13]) + fabsf(x[14] - y[14]);
}

static inline void zox_tst_rebuild_hat_set(ecs* world, entity character, entity hat) {
    if (hat) {
        zox_set_link(world, zox_tst_rebuild_hat_slot, DataLink, hat);
    } else {
        zox_unlink(world, zox_tst_rebuild_hat_slot, DataLink, zox_tst_rebuild_hat);
    }
    zox_setv(character, BodyDirty, zox_generate_body_start);
}

byte zox_tst_rebuild_ready(ecs* world) {
    if (!zox_tst_rebuild_stage) {
        zox_tst_held_stage = 0;
        zox_tst_rebuild_stage = 1;
    }
    if (zox_tst_rebuild_stage == 1) {
        zox_tst_waiting = "a cookie held";
        if (!zox_tst_held_item_ready(world) || !zox_valid(zox_tst_held_mesh)) {
            return 0;
        }
        const entity character = zox_tst_player_character(world);
        const entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
        entity slots[8];
        const uint slots_length = get_equip_slots_in_children(world, chest_slot, zox_id(HatSlot), slots, 8);
        zox_tst_rebuild_hat_slot = slots_length ? slots[0] : 0;
        zox_tst_rebuild_hat = zox_valid(zox_tst_rebuild_hat_slot) ? zox_get_link(world, zox_tst_rebuild_hat_slot, DataLink) : 0;
        if (!zox_valid(zox_tst_rebuild_hat)) {
            zox_tst_waiting = "the player's hat";
            return 0;
        }
        const entity skeleton = get_skeleton(world, character);
        zox_tst_rebuild_hat_bones = zox_get(skeleton, BoneLinks)->length;
        zox_tst_rebuild_look = zox_getv(zox_get_link(world, character, CameraLink), LocalRotation3D);
        zox_tst_rebuild_stage = 2;
        zox_tst_rebuild_frame = 0;
        zox_tst_rebuild_hat_set(world, character, 0);
        zox_log("[tst] bone_rebuild: the hat off, bones [%i]", zox_tst_rebuild_hat_bones);
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    zox_tst_waiting = "the hat off and on";
    zox_tst_progress(zox_tst_rebuild_frame);
    if (zox_tst_rebuild_frame == zox_tst_rebuild_off_frames) {
        zox_tst_rebuild_hat_set(world, character, zox_tst_rebuild_hat);
        zox_log("[tst] bone_rebuild: the hat on");
    }
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    // the bones: each still there at its index, none jumping
    int distinct = 0;
    float max_jump = 0;
    int jump_bone = -1;
    entity slots[64];
    const int length = bones->length < 64 ? bones->length : 64;
    const float3 origin = zox_getv(character, Position3D);
    for (int i = 0; i < length; i++) {
        const entity bone = bones->value[i];
        slots[i] = zox_valid(bone) ? zox_getv(bone, BoneSlot) : 0;
        byte first = 1;
        for (int j = 0; j < i && first; j++) {
            first = bones->value[j] != bone;
        }
        distinct += first;
        if (!zox_valid(bone) || !zox_has(bone, TransformMatrix)) {
            continue;
        }
        float4x4 m = zox_getv(bone, TransformMatrix);
        float* t = (float*) &m;
        t[12] -= origin.x;
        t[13] -= origin.y;
        t[14] -= origin.z;
        if (i < zox_tst_rebuild_last_length) {
            const float d = zox_tst_rebuild_difference(m, zox_tst_rebuild_last[i]);
            if (d > max_jump) {
                max_jump = d;
                jump_bone = i;
            }
        }
        zox_tst_rebuild_last[i] = m;
    }
    zox_tst_rebuild_last_length = length;
    zox_tst_rebuild_off_seen |= distinct && distinct < zox_tst_rebuild_hat_bones;
    for (int k = 0; k < zox_tst_rebuild_slots_length; k++) {
        const entity slot = zox_tst_rebuild_slots[k];
        int now = -1;
        for (int i = 0; i < length && now < 0 && slot; i++) {
            if (slots[i] == slot) {
                now = i;
            }
        }
        // the first index a slot's bone is at (its others are a gone bone's, its parent's)
        int before = k;
        for (int j = 0; j < k; j++) {
            if (zox_tst_rebuild_slots[j] == slot) {
                before = j;
                break;
            }
        }
        if (slot && now >= 0 && now != before) {
            zox_tst_rebuild_moved++;
            zox_log("[tst]   bone of index [%i] moved to [%i] at frame [%i]", before, now, zox_tst_rebuild_frame);
            break;
        }
    }
    memcpy(zox_tst_rebuild_slots, slots, sizeof(entity) * length);
    zox_tst_rebuild_slots_length = length;
    // the meshes
    int drawn = 0;
    char meshes[200] = "";
    iter it = zox_children(world, skeleton);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity mesh = it.entities[i];
            if (!zox_has(mesh, SkeletonMesh)) {
                continue;
            }
            const byte can = !zox_getv(mesh, RenderDisabled) && zox_get(mesh, MeshIndicies)->length &&
                zox_getv(mesh, MeshGPULink).x && zox_getv(mesh, BoneIndexGPULink) && !zox_has(mesh, Disabled);
            drawn += can;
            // the drawn mesh is never rebuilt in place (its index count over old gpu buffers), and an old one is
            //  never given weights from the new voxels
            const int indicies = zox_get(mesh, MeshIndicies)->length;
            if (can && (zox_has(mesh, BuildMesh) ||
                (zox_has(mesh, SkeletonMeshStale) && zox_has(mesh, SkeletonMeshDirty)) ||
                (mesh == zox_tst_rebuild_drawn_mesh && indicies != zox_tst_rebuild_drawn_length)))
            {
                zox_tst_rebuild_unready++;
                zox_log("[tst]   drawn mesh changed under it at frame [%i]", zox_tst_rebuild_frame);
            }
            if (can) {
                zox_tst_rebuild_meshes += mesh != zox_tst_rebuild_drawn_mesh;
                zox_tst_rebuild_drawn_mesh = mesh;
                zox_tst_rebuild_drawn_length = indicies;
            }
            char line[80];
            snprintf(line, sizeof(line), " [d%i %s%s%s%s%s%s%s i%i]", zox_getv(mesh, RenderDepth), can ? "DRAW" : "-",
                zox_has(mesh, Disabled) ? " Disabled" : "", zox_has(mesh, BuildMesh) ? " Build" : "",
                zox_has(mesh, BuildMeshWeights) ? " BuildW" : "", zox_has(mesh, SkeletonMeshDirty) ? " SDirty" : "",
                zox_has(mesh, MeshDirty) ? " MDirty" : "", zox_has(mesh, SkeletonMeshStale) ? " Stale" : "",
                indicies);
            strncat(meshes, line, sizeof(meshes) - strlen(meshes) - 1);
        }
    }
    // the held item and the look (only while the rebuilds settle: later the player may look around)
    const entity hand = zox_get_link(world, skeleton, HandBoneLink);
    const byte held = zox_valid(hand) && zox_valid(zox_get_child_by_id(world, hand, zox_id(HeldAction)));
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    const entity camera = zox_get_link(world, character, CameraLink);
    const float4 c = zox_getv(camera, LocalRotation3D);
    const float4 l = zox_tst_rebuild_look;
    const byte look = zox_get_parent(world, camera) == head &&
        fabsf(c.x - l.x) + fabsf(c.y - l.y) + fabsf(c.z - l.z) + fabsf(c.w - l.w) < 0.01f;
    const byte settling = zox_tst_rebuild_frame % zox_tst_rebuild_off_frames < 30;
    if (settling) {
        zox_tst_rebuild_look_lost += !look;
    }
    if (max_jump > 0.5f && jump_bone >= 0) {
        zox_log("[tst]   jump bone [%i] [%s] [%.3f]", jump_bone, zox_get_name(bones->value[jump_bone]), max_jump);
    }
    zox_log("[tst]   frame [%i] body [%i] bones [%i/%i] jump [%.3f] held [%i] look [%i] drawn [%i]%s", zox_tst_rebuild_frame,
        zox_getv(character, BodyDirty), distinct, bones->length, max_jump, held, look, drawn, meshes);
    zox_tst_rebuild_undrawn += !drawn;
    zox_tst_rebuild_item_gone += !held;
    zox_tst_rebuild_jumps += max_jump > 0.5f;
    zox_tst_rebuild_frame++;
    return zox_tst_rebuild_frame > zox_tst_rebuild_frames;
}

void zox_tst_rebuild_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const BoneLinks* bones = zox_get(get_skeleton(world, character), BoneLinks);
    zox_tst_check(zox_tst_rebuild_off_seen, "the hat's bone was never gone (bones [%i])", zox_tst_rebuild_hat_bones);
    zox_tst_check(bones->length <= zox_bones_max, "[%i] bones, past the shader's [%i]", bones->length, zox_bones_max);
    zox_tst_check(zox_tst_rebuild_meshes >= 3, "the skeleton mesh swapped [%i] times, not for the hat off and on",
        zox_tst_rebuild_meshes - 1);
    zox_tst_check(!zox_tst_rebuild_moved, "a bone moved index on [%i] frames", zox_tst_rebuild_moved);
    zox_tst_check(!zox_tst_rebuild_unready, "a drawn skeleton mesh changed under it on [%i] frames", zox_tst_rebuild_unready);
    zox_tst_check(!zox_tst_rebuild_undrawn, "the skeleton had no mesh to draw for [%i] frames", zox_tst_rebuild_undrawn);
    zox_tst_check(!zox_tst_rebuild_item_gone, "the held item was gone for [%i] frames", zox_tst_rebuild_item_gone);
    zox_tst_check(!zox_tst_rebuild_look_lost, "the camera lost its look (or the head) for [%i] frames", zox_tst_rebuild_look_lost);
    zox_tst_check(!zox_tst_rebuild_jumps, "a bone's transform jumped on [%i] frames", zox_tst_rebuild_jumps);
}

void zox_tsts_bone_rebuild(ecs* world) {
    zox_tst_add("bone_rebuild", zox_tst_rebuild_ready, zox_tst_rebuild_check, 120);
    zox_tst_set_setup("bone_rebuild", zox_tst_level_setup, 0);
}
