// tst: place_hit, placing a block is instant: the press starts the place swing, the block goes in at the swing's hit
//  (items/eve/place.c, its hit mark), not before
//  - gives the player blocks (a realm block item), looks down at the ground, uses one (ActivateBegin, as its key
//    does), its swing slowed (2s)
//  - the swing starts at the press, a place, of that item; before its hit the stack's the same, at it one less (the
//    block placed), within a frame or two of the mark's time
//  - run: bash bsh/tst.sh place_hit
#define zox_tst_phit_seconds 2.0
byte zox_tst_phit_stage = 0;
double zox_tst_phit_at = 0;
entity zox_tst_phit_item = 0;
byte zox_tst_phit_swung = 0;
double zox_tst_phit_mark = 0;       // the hit mark's time
byte zox_tst_phit_quantity = 0;     // before the press
int zox_tst_phit_early = 0;         // placed before the hit
double zox_tst_phit_late = -1;      // the place after its mark, seconds

static inline entity zox_tst_phit_block_item(ecs* world, entity realm) {
    entity found = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            if (!found && zox_has(e, ItemBlock) && zox_valid(zox_get_link(world, e, BlockLink))) {
                found = e;
            }
        }
    }
    return found;
}

byte zox_tst_place_hit_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    const entity realm_item = zox_tst_phit_block_item(world, zox_get_link(world, character, RealmLink));
    zox_tst_progress(zox_tst_phit_stage);
    if (zox_tst_phit_stage == 0) {
        zox_tst_waiting = "blocks given";
        if (!zox_valid(realm_item) || !add_item_to_slot_manager(world, actionbar, character, realm_item, 4)) {
            return 1;   // the checks say so
        }
        zox_tst_phit_stage = 1;
        zox_tst_phit_at = zox_current_time;
        return 0;
    }
    // look down at the ground
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (zox_valid(camera)) {
        zox_setv(camera, LocalRotation3D, euler_to_quaternion((float3) { -1.1f, is_camera_positive_z ? M_PI : 0, 0 }));
    }
    if (zox_current_time - zox_tst_phit_at > 10) {
        return 1;
    }
    if (zox_tst_phit_stage == 1) {
        zox_tst_waiting = "the blocks in the actionbar, aimed at the ground";
        entity slots[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, actionbar, slots, zox_children_capacity, zox_id(Slot));
        for (uint i = 0; i < count; i++) {
            const entity data = zox_get_link(world, slots[i], DataLink);
            if (zox_valid(data) && zox_getp(world, data) == realm_item) {
                zox_tst_phit_item = data;
            }
        }
        const RaycastVoxelData* ray = zox_get(character, RaycastVoxelData);
        if (!zox_valid(zox_tst_phit_item) || ray->result != rayhit_terrain || ray->distance > zox_getv(character, RaycastRange)) {
            return 0;
        }
        zox_tst_phit_quantity = zox_getv(zox_tst_phit_item, Quantity);
        zox_setv(zox_tst_phit_item, SwingTime, zox_tst_phit_seconds);
        zox_setv(zox_tst_phit_item, ActivateBegin, zox_dirty_trigger);
        zox_tst_phit_at = zox_current_time;
        zox_tst_phit_stage = 2;
        return 0;
    }
    const entity item = zox_tst_phit_item;
    const int placed = zox_valid(item) ? zox_tst_phit_quantity - zox_getv(item, Quantity) : 1;
    if (zox_tst_phit_stage == 2) {
        zox_tst_waiting = "the place swing";
        const double start = zox_getv(skeleton, SwingStart);
        if (start >= zox_tst_phit_at && zox_getv(skeleton, SwingAction) == item) {
            zox_tst_phit_swung = zox_getv(skeleton, SwingKind) == zox_swing_place;
            zox_tst_phit_mark = start + swings[zox_swing_place].marks[0] * zox_getv(skeleton, SwingSpeed);
            zox_tst_phit_stage = 3;
        }
        return 0;
    }
    if (zox_current_time < zox_tst_phit_mark) {
        zox_tst_waiting = "the swing's hit";
        zox_tst_phit_early = placed;
        return 0;
    }
    zox_tst_waiting = "the block placed";
    if (placed) {
        zox_tst_phit_late = zox_current_time - zox_tst_phit_mark;
        return 1;
    }
    return zox_current_time - zox_tst_phit_mark > 1;
}

void zox_tst_place_hit_check(ecs* world) {
    zox_log("[tst] place_hit: swung [%i], placed before the hit [%i], at it [%.3f]s after its mark", zox_tst_phit_swung,
        zox_tst_phit_early, zox_tst_phit_late);
    zox_tst_check(zox_valid(zox_tst_phit_item), "no blocks given to place");
    zox_tst_check(zox_tst_phit_stage >= 3, "it never swung (aimed at the ground? stage [%i])", zox_tst_phit_stage);
    zox_tst_check(zox_tst_phit_swung, "the swing at the press isn't a place");
    zox_tst_check(zox_tst_phit_early == 0, "placed [%i] before the swing's hit", zox_tst_phit_early);
    zox_tst_check(zox_tst_phit_late >= 0 && zox_tst_phit_late < 0.1, "the block went in [%f]s from the hit (or never)", zox_tst_phit_late);
}

void zox_tsts_place_hit(ecs* world) {
    zox_tst_add("place_hit", zox_tst_place_hit_ready, zox_tst_place_hit_check, 60);
}
