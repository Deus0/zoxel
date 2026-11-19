// tst: eat_swing, eating a cookie is instant: the press swings the arm to the mouth (items/consumables, zox_swing_eat)
//    and it's eaten at the swing's first bite, not before
//  - gives the player cookies (held_item.c's); pressed at full health: no swing, none eaten (it played the eat swing
//    and ate nothing: eating forever); then hurt, begins using one (ActivateBegin, as its key does)
//  - the swing starts at the press, eating, over the cookie's swing time (SwingTime)
//  - it chews (consumables/eve/eat.c, on the swing's marks): each of its bites (a munch, crumbs) once; the stack's the
//    same before the first bite, one less after
//  - then the other two eaten: the stack's gone (the item deleted), cookies don't eat forever
//  - its crumbs from its model's palette (particles3/fun/burst.c): the held cookie has one
//  - saves tst/eat_swing.ppm after the second bite (the hand at the mouth, crumbs falling)
//  - run: bash bsh/tst.sh eat_swing
byte zox_tst_eat_stage = 0;
entity zox_tst_eat_item = 0;
byte zox_tst_eat_swung = 0;
byte zox_tst_eat_kind = 255;
float zox_tst_eat_time = 0;
double zox_tst_eat_at = 0;
byte zox_tst_eat_done = 0;      // the swing seen to its end
byte zox_tst_eat_quantity = 0;  // before the press
int zox_tst_eat_early = 0;      // eaten before the first bite
int zox_tst_eat_eaten = 0;      // after the swing
byte zox_tst_eat_gone = 0;      // all eaten (the rest one at a time): the stack's gone, not eaten forever
int zox_tst_eat_presses = 1;
byte zox_tst_eat_full_swung = 0;    // pressed at full health: swung (it can't eat, the arm went to the mouth anyway)
int zox_tst_eat_full_eaten = 0;
double zox_tst_eat_full_at = 0;
float zox_tst_eat_expected = 0;     // its swing time, read before it's all eaten (deleted)
int zox_tst_eat_palette = 0;        // its model's colours, the same
uint zox_tst_eat_bites = 0;         // bites in the first swing

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
        if (!zox_valid(cookie) || !add_item_to_slot_manager(world, actionbar, character, cookie, 3)) {
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
        if (!zox_valid(zox_tst_eat_item)) {
            return 0;
        }
        const entity stat = get_user_stat_from_realm(world, character, zox_get_link(world, zox_tst_eat_item, ConsumeStat));
        // first at full health: it can't eat, so no swing either
        if (!zox_tst_eat_full_at) {
            if (zox_valid(stat) && zox_has(stat, StatValueMax)) {
                zox_setv(stat, StatValue, zox_getv(stat, StatValueMax));
            }
            zox_tst_eat_quantity = zox_getv(zox_tst_eat_item, Quantity);
            zox_setv(zox_tst_eat_item, ActivateBegin, zox_dirty_trigger);
            zox_tst_eat_full_at = zox_current_time;
            return 0;
        }
        zox_tst_waiting = "a press at full health";
        if (zox_getv(skeleton, SwingAction) == zox_tst_eat_item && zox_getv(skeleton, SwingStart) >= zox_tst_eat_full_at) {
            zox_tst_eat_full_swung = 1;
        }
        zox_tst_eat_full_eaten = zox_tst_eat_quantity - zox_getv(zox_tst_eat_item, Quantity);
        if (zox_current_time - zox_tst_eat_full_at < zox_getv(zox_tst_eat_item, CooldownTime) + 0.2) {
            return 0;
        }
        if (zox_valid(zox_tst_eat_item)) {
            // hurt, so it eats
            if (zox_valid(stat)) {
                zox_setv(stat, StatValue, 1);
            }
            zox_tst_eat_quantity = zox_getv(zox_tst_eat_item, Quantity);
            zox_setv(zox_tst_eat_item, ActivateBegin, zox_dirty_trigger);
            zox_tst_eat_stage = 2;
            zox_tst_eat_at = zox_current_time;
        }
        return 0;
    }
    if (zox_current_time - zox_tst_eat_at > 10) {
        return 1;
    }
    const int eaten = zox_valid(zox_tst_eat_item) ? zox_tst_eat_quantity - zox_getv(zox_tst_eat_item, Quantity) : 0;
    const double start = zox_valid(skeleton) ? zox_getv(skeleton, SwingStart) : 0;
    if (zox_tst_eat_stage == 2) {
        zox_tst_waiting = "the eat swing";
        if (start >= zox_tst_eat_at && zox_getv(skeleton, SwingAction) == zox_tst_eat_item) {
            zox_tst_eat_swung = 1;
            zox_tst_eat_kind = zox_getv(skeleton, SwingKind);
            zox_tst_eat_time = zox_getv(skeleton, SwingSpeed);
            zox_tst_eat_expected = zox_getv(zox_tst_eat_item, SwingTime);
            const entity model = zox_get_link(world, zox_tst_eat_item, ModelLink);
            zox_tst_eat_palette = zox_valid(model) && zox_has(model, ColorRGBs) ? zox_get(model, ColorRGBs)->length : 0;
            zox_tst_eat_stage = 3;
        }
        return 0;
    }
    zox_tst_waiting = "the chewing";
    const float p = (float) ((zox_current_time - start) / zox_tst_eat_time);
    if (zox_tst_eat_stage <= 4 && p < swings[zox_swing_eat].marks[0]) {
        zox_tst_eat_early = eaten;
    }
    if (zox_tst_eat_stage == 3 && p > 0.7f) {
        // just after the second bite: the hand at the mouth, crumbs falling
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("eat_swing.ppm"));
        zox_tst_eat_stage = 4;
    } else if (zox_tst_eat_stage <= 4 && p > 1) {
        zox_tst_eat_done = 1;
        zox_tst_eat_eaten = eaten;
        zox_tst_eat_bites = zox_eat_bites;
        zox_tst_eat_stage = 5;
    }
    if (zox_tst_eat_stage == 5) {
        // the rest, one at a time as each cools down, until the stack's gone
        zox_tst_waiting = "the rest eaten";
        if (!zox_valid(zox_tst_eat_item)) {
            zox_tst_eat_gone = 1;
            return 1;
        }
        const entity stat = get_user_stat_from_realm(world, character, zox_get_link(world, zox_tst_eat_item, ConsumeStat));
        if (zox_valid(stat)) {
            zox_setv(stat, StatValue, 1);
        }
        if (!zox_getv(zox_tst_eat_item, CooldownAt) && !zox_getv(zox_tst_eat_item, ActivateBegin) &&
            zox_current_time - start > zox_getv(zox_tst_eat_item, CooldownTime) + 0.1) {
            zox_setv(zox_tst_eat_item, ActivateBegin, zox_dirty_trigger);
            zox_tst_eat_presses++;
        }
    }
    return 0;
}

void zox_tst_eat_swing_check(ecs* world) {
    const float expected = zox_tst_eat_expected;
    zox_log("[tst] eat_swing: swung at the press [%i] kind [%i] over [%.3f]s of [%.3f], eaten before the first bite [%i], after [%i]",
        zox_tst_eat_swung, zox_tst_eat_kind, zox_tst_eat_time, expected, zox_tst_eat_early, zox_tst_eat_eaten);
    zox_tst_check(zox_tst_eat_item, "no cookie given to eat");
    zox_tst_check(zox_tst_eat_swung, "the swing didn't start at the press");
    zox_tst_check(zox_tst_eat_kind == zox_swing_eat, "the swing isn't eating [%i]", zox_tst_eat_kind);
    zox_tst_check(fabsf(zox_tst_eat_time - expected) < 0.001f, "the swing isn't over the food's swing time [%f of %f]", zox_tst_eat_time, expected);
    const byte bites = swings[zox_swing_eat].marks_count;
    const int palette = zox_tst_eat_palette;
    zox_log("[tst] eat_swing: chewed [%u] bites of [%i], crumbs from [%i] colours", zox_tst_eat_bites, bites, palette);
    zox_tst_check(zox_tst_eat_done, "the eat swing never ended");
    zox_tst_check(zox_tst_eat_early == 0, "eaten [%i] before the first bite", zox_tst_eat_early);
    zox_tst_check(zox_tst_eat_eaten == 1, "eaten [%i], not one", zox_tst_eat_eaten);
    zox_log("[tst] eat_swing: at full health swung [%i] eaten [%i]", zox_tst_eat_full_swung, zox_tst_eat_full_eaten);
    zox_tst_check(!zox_tst_eat_full_swung && !zox_tst_eat_full_eaten, "at full health it swung [%i] or ate [%i]",
        zox_tst_eat_full_swung, zox_tst_eat_full_eaten);
    zox_log("[tst] eat_swing: the rest eaten, the stack gone [%i] after [%i] presses", zox_tst_eat_gone, zox_tst_eat_presses);
    zox_tst_check(zox_tst_eat_gone, "the cookies never ran out (after [%i] presses)", zox_tst_eat_presses);
    zox_tst_check(zox_tst_eat_bites == bites, "chewed [%u] bites, not [%i]", zox_tst_eat_bites, bites);
    zox_tst_check(palette > 0, "the cookie's model has no colours for its crumbs");
}

void zox_tsts_eat_swing(ecs* world) {
    zox_tst_add("eat_swing", zox_tst_eat_swing_ready, zox_tst_eat_swing_check, 60);
}
