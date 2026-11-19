// tst: eat_swing, eating a cookie swings the arm to the mouth from the item's warmup (items/sys/swing.c, zox_swing_eat)
//  - gives the player a cookie (held_item.c's), begins using it (ActivateBegin, as its key does)
//  - in its warmup the skeleton's swing is eating, started at the warmup's start, over warmup + cooldown
//  - run: bash bsh/tst.sh eat_swing
byte zox_tst_eat_stage = 0;
entity zox_tst_eat_item = 0;
byte zox_tst_eat_swung = 0;
byte zox_tst_eat_kind = 255;
float zox_tst_eat_time = 0;
double zox_tst_eat_at = 0;

byte zox_tst_eat_swing_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    zox_tst_progress(zox_tst_eat_stage);
    if (zox_tst_eat_stage == 0) {
        zox_tst_waiting = "a cookie given";
        const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
        const entity cookie = zox_tst_held_cookie(world, zox_get_link(world, character, RealmLink));
        if (!zox_valid(cookie) || !add_item_to_slot_manager(world, actionbar, character, cookie, 1)) {
            return 1;   // the checks say so
        }
        zox_tst_eat_stage = 1;
        return 0;
    }
    if (zox_tst_eat_stage == 1) {
        zox_tst_waiting = "the cookie in the actionbar";
        const entity cookie = zox_tst_held_cookie(world, zox_get_link(world, character, RealmLink));
        const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
        entity slots[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, actionbar, slots, zox_children_capacity, zox_id(Slot));
        for (uint i = 0; i < count; i++) {
            const entity data = zox_get_link(world, slots[i], DataLink);
            if (zox_valid(data) && zox_getp(world, data) == cookie) {
                zox_tst_eat_item = data;
            }
        }
        if (zox_valid(zox_tst_eat_item)) {
            zox_setv(zox_tst_eat_item, ActivateBegin, zox_dirty_trigger);
            zox_tst_eat_stage = 2;
            zox_tst_eat_at = zox_current_time;
        }
        return 0;
    }
    zox_tst_waiting = "the cookie's warmup";
    const double warmup_at = zox_getv(zox_tst_eat_item, WarmupAt);
    if (warmup_at && zox_valid(skeleton) && zox_getv(skeleton, SwingStart) == warmup_at) {
        zox_tst_eat_swung = 1;
        zox_tst_eat_kind = zox_getv(skeleton, SwingKind);
        zox_tst_eat_time = zox_getv(skeleton, SwingSpeed);
        return 1;
    }
    return zox_current_time - zox_tst_eat_at > 3;
}

void zox_tst_eat_swing_check(ecs* world) {
    const entity e = zox_tst_eat_item;
    const float expected = zox_valid(e) ? zox_getv(e, WarmupTime) + zox_getv(e, CooldownTime) : 0;
    zox_log("[tst] eat_swing: swung in the warmup [%i] kind [%i] over [%.3f]s of [%.3f]", zox_tst_eat_swung, zox_tst_eat_kind, zox_tst_eat_time, expected);
    zox_tst_check(zox_valid(e), "no cookie given to eat");
    zox_tst_check(zox_tst_eat_swung, "the swing didn't start with the cookie's warmup");
    zox_tst_check(zox_tst_eat_kind == zox_swing_eat, "the swing isn't eating [%i]", zox_tst_eat_kind);
    zox_tst_check(fabsf(zox_tst_eat_time - expected) < 0.001f, "the swing isn't over warmup and cooldown [%f of %f]", zox_tst_eat_time, expected);
}

void zox_tsts_eat_swing(ecs* world) {
    zox_tst_add("eat_swing", zox_tst_eat_swing_ready, zox_tst_eat_swing_check, 60);
}
