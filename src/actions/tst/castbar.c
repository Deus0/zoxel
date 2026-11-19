// tst: castbar, casting (an action's warmup) shows a castbar above the actionbar, filling over its cast time
//  - the player given the realm's shoot skill (it casts: a warmup), selected, its cast slowed (2s), begun
//    (ActivateBegin, as its key does)
//  - before: the castbar's hidden; in the cast it shows, its action the shoot, its fill rising through the middle;
//    after: hidden again (AutoHide)
//  - the skills' tooltips say their type: the punch instant (SkillInstant), the shoot cast [0.50s]
//  - saves tst/castbar.ppm mid cast
//  - run: bash bsh/tst.sh castbar
#define zox_tst_cast_seconds 2.0
byte zox_tst_cast_stage = 0;
double zox_tst_cast_at = 0;
entity zox_tst_cast_skill = 0;
entity zox_tst_cast_bar = 0;
byte zox_tst_cast_hidden_before = 0;
byte zox_tst_cast_shown = 0;        // seen in the cast
byte zox_tst_cast_named = 0;        // its action the cast's
float zox_tst_cast_low = 1, zox_tst_cast_high = 0;  // its fill in the cast
byte zox_tst_cast_hidden_after = 0;
byte zox_tst_cast_shot = 0;
char zox_tst_cast_punch_text[256];  // the punch's tooltip (instant)
char zox_tst_cast_shoot_text[256];  // the shoot's (cast)
byte zox_tst_cast_punch_instant = 0;
byte zox_tst_cast_shoot_instant = 1;

static inline entity zox_tst_cast_realm_shoot(ecs* world, entity realm) {
    entity found = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            if (!found && zox_has(it.entities[i], Shoot)) {
                found = it.entities[i];
            }
        }
    }
    return found;
}

byte zox_tst_castbar_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = zox_players[0];
    const entity character = zox_tst_player_character(world);
    zox_tst_progress(zox_tst_cast_stage * 100 + (int) ((zox_current_time - zox_tst_cast_at) * 10));
    if (zox_tst_cast_stage == 0) {
        zox_tst_waiting = "the actionbar's castbar";
        const entity canvas = zox_get_link(world, player, CanvasLink);
        const entity menu = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(MenuActions)) : 0;
        zox_tst_cast_bar = zox_valid(menu) ? zox_get_child_by_id(world, menu, zox_id(Castbar)) : 0;
        const entity realm_shoot = zox_tst_cast_realm_shoot(world, zox_get_link(world, character, RealmLink));
        const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
        const entity slot = zox_valid(actionbar) ? zox_get_empty_slot(world, actionbar) : 0;
        if (!zox_valid(zox_tst_cast_bar) || !zox_valid(realm_shoot) || !zox_valid(slot)) {
            return 0;
        }
        zox_tst_cast_skill = spawn_user_skill(world, character, realm_shoot);
        // the tooltips say which: the punch instant, the shoot cast
        entity children[zox_children_capacity];
        const uint count = zox_get_children(world, character, children, zox_children_capacity);
        for (uint i = 0; i < count; i++) {
            if (zox_has(children[i], Melee)) {
                skill_tooltip_text(world, children[i], zox_tst_cast_punch_text, sizeof(zox_tst_cast_punch_text));
                zox_tst_cast_punch_instant = zox_has(children[i], SkillInstant);
            }
        }
        skill_tooltip_text(world, realm_shoot, zox_tst_cast_shoot_text, sizeof(zox_tst_cast_shoot_text));
        zox_tst_cast_shoot_instant = zox_has(realm_shoot, SkillInstant);
        zox_set_link(world, slot, DataLink, zox_tst_cast_skill);
        zox_setv(character, ActionIndex, zox_getv(slot, ChildIndex));
        zox_tst_cast_stage = 1;
        zox_tst_cast_at = zox_current_time;
        return 0;
    }
    if (zox_current_time - zox_tst_cast_at > 10) {
        return 1;   // the checks say what didn't happen
    }
    const entity bar = zox_tst_cast_bar;
    const entity skill = zox_tst_cast_skill;
    if (zox_tst_cast_stage == 1) {
        zox_tst_waiting = "the shoot selected";
        if (zox_getv(character, ActiveAction) != skill || zox_current_time - zox_tst_cast_at < 1) {
            return 0;
        }
        zox_tst_cast_hidden_before = zox_has(bar, AutoHidden);
        zox_setv(skill, WarmupTime, zox_tst_cast_seconds);
        zox_setv(skill, ActivateBegin, zox_dirty_trigger);
        zox_tst_cast_stage = 2;
        zox_tst_cast_at = zox_current_time;
        return 0;
    }
    const double cast_at = zox_getv(skill, WarmupAt);
    if (zox_tst_cast_stage == 2) {
        zox_tst_waiting = "the cast";
        if (cast_at) {
            zox_tst_cast_stage = 3;
        }
        return 0;
    }
    if (zox_tst_cast_stage == 3) {
        zox_tst_waiting = "the cast to end";
        if (!cast_at) {
            zox_tst_cast_stage = 4;
            zox_tst_cast_at = zox_current_time;
            return 0;
        }
        const double done = (zox_current_time - cast_at) / zox_tst_cast_seconds;
        if (done > 0.6 && !zox_tst_cast_shot) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("castbar.ppm"));
            zox_tst_cast_shot = 1;
        }
        if (done > 0.25) {
            zox_tst_cast_shown |= !zox_has(bar, AutoHidden);
            zox_tst_cast_named |= zox_getv(bar, CastAction) == skill;
            const float level = zox_getv(bar, BarLevel);
            zox_tst_cast_low = level < zox_tst_cast_low ? level : zox_tst_cast_low;
            zox_tst_cast_high = level > zox_tst_cast_high ? level : zox_tst_cast_high;
        }
        return 0;
    }
    zox_tst_waiting = "the castbar hidden after";
    if (zox_current_time - zox_tst_cast_at < 1) {
        return 0;
    }
    zox_tst_cast_hidden_after = zox_has(bar, AutoHidden);
    return 1;
}

void zox_tst_castbar_check(ecs* world) {
    zox_log("[tst] castbar: hidden before [%i], shown [%i] for the cast [%i], fill [%.2f to %.2f], hidden after [%i]",
        zox_tst_cast_hidden_before, zox_tst_cast_shown, zox_tst_cast_named, zox_tst_cast_low, zox_tst_cast_high,
        zox_tst_cast_hidden_after);
    zox_tst_check(zox_valid(zox_tst_cast_bar), "no castbar on the actionbar");
    zox_log("[tst] castbar: punch tooltip [%s]", zox_tst_cast_punch_text);
    zox_log("[tst] castbar: shoot tooltip [%s]", zox_tst_cast_shoot_text);
    zox_tst_check(zox_tst_cast_punch_instant && strstr(zox_tst_cast_punch_text, "instant"), "the punch isn't shown instant");
    zox_tst_check(!zox_tst_cast_shoot_instant && strstr(zox_tst_cast_shoot_text, "cast [0.50s]"), "the shoot isn't shown cast");
    zox_tst_check(zox_tst_cast_stage >= 4, "the cast never began and ended (stage [%i])", zox_tst_cast_stage);
    zox_tst_check(zox_tst_cast_hidden_before, "the castbar showed before a cast");
    zox_tst_check(zox_tst_cast_shown, "the castbar didn't show in the cast");
    zox_tst_check(zox_tst_cast_named, "the castbar isn't the cast's");
    zox_tst_check(zox_tst_cast_low < 0.45f && zox_tst_cast_high > 0.85f, "the castbar didn't fill over the cast [%f to %f]",
        zox_tst_cast_low, zox_tst_cast_high);
    zox_tst_check(zox_tst_cast_hidden_after, "the castbar stayed after the cast");
}

void zox_tsts_castbar(ecs* world) {
    zox_tst_add("castbar", zox_tst_castbar_ready, zox_tst_castbar_check, 60);
}
