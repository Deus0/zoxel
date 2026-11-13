// tst: held_item, a vox item (a cookie) held in the hand sits in it: its mesh centred on the hand bone
//  - gives the player a cookie, selects it, the held mesh (HeldAction on the hand bone) spawns
//  - its vertices' middle, scaled, is near its origin (its model's voxels centred, not its grid)
//  - saves tst/held_item.ppm, the hand in first person (--nohud from tst.sh)
//  - held_block: a block item, to compare (tst/held_block.ppm)
//  - run: bash bsh/tst.sh held_item
byte zox_tst_held_stage = 0;
double zox_tst_held_time = 0;
entity zox_tst_held_mesh = 0;

byte zox_tst_held_block = 0;     // held_block: a block item instead, to compare

static inline entity zox_tst_held_cookie(ecs* world, entity realm) {
    entity found = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const byte is_block = zox_has(it.entities[i], Item) && zox_valid(zox_get_link(world, it.entities[i], BlockLink));
            if (zox_tst_held_block ? is_block : (zox_has(it.entities[i], ItemVox) && zox_has(it.entities[i], ItemConsumable))) {
                found = it.entities[i];
            }
        }
    }
    return found;
}

byte zox_tst_held_item_ready(ecs* world) {
    if (!zox_tst_held_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_held_stage = 1;
        zox_tst_held_time = zox_current_time;
    }
    zox_tst_progress(zox_tst_held_stage);
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    if (zox_tst_held_stage == 1) {
        zox_tst_waiting = "a cookie in the actionbar";
        const entity cookie = zox_tst_held_cookie(world, zox_get_link(world, character, RealmLink));
        if (!zox_valid(cookie) || !add_item_to_slot_manager(world, actionbar, character, cookie, 1)) {
            zox_tst_check(0, "no cookie to give, or the actionbar is full");
            return 1;
        }
        zox_tst_held_stage = 2;
        return 0;
    }
    if (zox_tst_held_stage == 2) {
        // select its slot
        zox_tst_waiting = "the cookie selected";
        const entity cookie = zox_tst_held_cookie(world, zox_get_link(world, character, RealmLink));
        entity slots[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, actionbar, slots, zox_children_capacity, zox_id(Slot));
        for (uint i = 0; i < count; i++) {
            const entity data = zox_get_link(world, slots[i], DataLink);
            if (zox_valid(data) && zox_getp(world, data) == cookie) {
                zox_setv(character, ActionIndex, zox_getv(slots[i], ChildIndex));
                zox_tst_held_stage = 3;
                zox_tst_held_time = zox_current_time;
            }
        }
        return 0;
    }
    // look a little down at the hand
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity head_camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (zox_valid(head_camera)) {
        zox_setv(head_camera, LocalRotation3D, euler_to_quaternion((float3) { -0.35f, is_camera_positive_z ? M_PI : 0, 0 }));
    }
    const entity hand = zox_valid(skeleton) ? zox_get_link(world, skeleton, HandBoneLink) : 0;
    zox_tst_held_mesh = zox_valid(hand) ? zox_get_child_by_id(world, hand, zox_id(HeldAction)) : 0;
    zox_tst_waiting = "the held cookie";
    if (zox_tst_held_stage == 3) {
        if (zox_current_time - zox_tst_held_time < 2) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(zox_tst_held_block ? "held_block.ppm" : "held_item.ppm"));
        zox_tst_held_stage = 4;
        return 0;
    }
    return !zox_screenshot_request[0];
}

byte zox_tst_held_block_ready(ecs* world) {
    zox_tst_held_block = 1;
    return zox_tst_held_item_ready(world);
}

void zox_tst_held_item_check(ecs* world) {
    zox_tst_check(zox_valid(zox_tst_held_mesh), "no held item on the hand");
    if (!zox_valid(zox_tst_held_mesh)) {
        return;
    }
    // its vertices' middle, scaled to the hand: where the cookie is from the hand bone
    const MeshVertices* vertices = zox_get(zox_tst_held_mesh, MeshVertices);
    float3 low = float3_single(1000000), high = float3_single(-1000000);
    for (int i = 0; i < vertices->length; i++) {
        low = (float3) { fminf(low.x, vertices->value[i].x), fminf(low.y, vertices->value[i].y), fminf(low.z, vertices->value[i].z) };
        high = (float3) { fmaxf(high.x, vertices->value[i].x), fmaxf(high.y, vertices->value[i].y), fmaxf(high.z, vertices->value[i].z) };
    }
    const float scale = zox_getv(zox_tst_held_mesh, LocalScale1);
    const float3 middle = float3_scale(float3_mid(low, high), scale);
    const float3 size = float3_scale(float3_sub(high, low), scale);
    // where its voxels' middle is from the hand: centred across it, out of it by half its depth
    const float3 local = zox_getv(zox_tst_held_mesh, LocalPosition3D);
    const float3 centre = float3_add(middle, local);
    const float off = sqrtf(centre.x * centre.x + centre.y * centre.y);
    const float out = -centre.z - size.z / 2;
    zox_log("[tst] [metric] held_item its middle [%.3f %.3f %.3f] from the hand, [%.3f] off centre, [%.3f] out of it, size [%.3f %.3f %.3f]",
        centre.x, centre.y, centre.z, off, out, size.x, size.y, size.z);
    zox_tst_check(vertices->length, "the held item has no mesh");
    zox_tst_check(off < 0.01f, "the held item's middle is [%.3f] off the hand's centre", off);
    zox_tst_check(out > 0 && out < 0.03f, "the held item is [%.3f] out of the hand, not just in front of it", out);
    // sized from its voxels, its mesh (a lower detail one) rounds up a voxel or so
    zox_tst_check(fabsf(float_max(size.x, float_max(size.y, size.z)) - 0.125f) < 0.01f, "the held item's biggest side is [%.3f], not a block's",
        float_max(size.x, float_max(size.y, size.z)));
}
