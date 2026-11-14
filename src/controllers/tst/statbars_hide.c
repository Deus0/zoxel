// tst: statbars_hide, the player's stat bars each hide when their stat doesn't change, pop back in when it does
//  (AutoHide on each bar, ui_poke), the panel itself stays
//  - its stats filled (nothing regenerates), waits for every bar to hide, shrunk to nothing
//  - hurts the player: its health bar shows again at full size, the others stay hidden
//  - run: bash bsh/tst.sh statbars_hide
byte zox_tst_hide_stage = 0;
double zox_tst_hide_time = 0;
byte zox_tst_hide_hid = 0;          // every bar hidden and shrunk once idle
float zox_tst_hide_health = -1;     // its health bar's scale once hurt
int zox_tst_hide_others = 0;        // the other bars still hidden then
int zox_tst_hide_bars = 0;
byte zox_tst_hide_panel = 0;        // the panel hid (it shouldn't)

static inline entity zox_tst_statbars(ecs* world) {
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    return zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(StatBars)) : 0;
}

// the panel's bars (AutoHide), how many are hidden, and the health bar
static inline int zox_tst_hide_scan(ecs* world, entity e, int* hidden, entity* health) {
    int count = 0;
    if (zox_has(e, AutoHide) && zox_has(e, Statbar)) {
        count++;
        *hidden += zox_has(e, AutoHidden) && zox_getv(e, LocalScale1) < 0.01f;
        const entity stat = zox_get_link(world, e, StatLink);
        if (zox_valid(stat) && zox_has(stat, StatHealth)) {
            *health = e;
        }
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            count += zox_tst_hide_scan(world, it.entities[i], hidden, health);
        }
    }
    return count;
}

byte zox_tst_statbars_hide_ready(ecs* world) {
    if (!zox_tst_hide_stage) {
        if (!zox_tst_player_spawn_ready(world) || !zox_valid(zox_tst_statbars(world))) {
            return 0;
        }
        // its stats full, nothing regenerates (energy and mana start low)
        const entity character = zox_get_link(world, dbg_player, CharacterLink);
        entity stats[zox_children_capacity];
        const uint stats_count = zox_get_children_by_id(world, character, stats, zox_children_capacity, zox_id(Stat));
        for (uint i = 0; i < stats_count; i++) {
            if (zox_has(stats[i], StatState)) {
                zox_setv(stats[i], StatValue, zox_getv(stats[i], StatValueMax));
            }
        }
        zox_tst_hide_stage = 1;
        zox_tst_hide_time = zox_current_time;
    }
    const entity panel = zox_tst_statbars(world);
    if (!zox_valid(panel)) {
        return 1;
    }
    zox_tst_progress(zox_tst_hide_stage);
    zox_tst_hide_panel |= zox_has(panel, AutoHidden);
    int hidden = 0;
    entity health = 0;
    zox_tst_hide_bars = zox_tst_hide_scan(world, panel, &hidden, &health);
    if (zox_tst_hide_stage == 1) {
        zox_tst_waiting = "the stat bars to hide";
        if (zox_tst_hide_bars && hidden == zox_tst_hide_bars) {
            zox_tst_hide_hid = 1;
            // hurt: its health changes, its bar pokes itself
            const entity character = zox_get_link(world, dbg_player, CharacterLink);
            const entity stat = zox_get_child_by_id(world, character, zox_id(StatHealth));
            zox_setv(stat, StatValue, zox_getv(stat, StatValue) - 1);
            zox_tst_hide_stage = 2;
            zox_tst_hide_time = zox_current_time;
        }
        return zox_current_time - zox_tst_hide_time > statbars_hide_time + 10;
    }
    zox_tst_waiting = "the health bar to show";
    if (zox_current_time - zox_tst_hide_time < 1) {
        return 0;
    }
    zox_tst_hide_health = zox_valid(health) && !zox_has(health, AutoHidden) ? zox_getv(health, LocalScale1) : 0;
    zox_tst_hide_others = hidden;
    return 1;
}

void zox_tst_statbars_hide_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] statbars_hide [%i] bars, [%i] others hidden after a hit", zox_tst_hide_bars, zox_tst_hide_others);
    zox_tst_check(zox_tst_hide_hid, "the stat bars didn't all hide after [%.0f] seconds idle", statbars_hide_time);
    zox_tst_check(fabsf(zox_tst_hide_health - 1) < 0.01f, "hurt, the health bar came back at [%.2f], not 1", zox_tst_hide_health);
    zox_tst_check(zox_tst_hide_others == zox_tst_hide_bars - 1, "hurt, [%i] of [%i] bars stayed hidden, all but health should",
        zox_tst_hide_others, zox_tst_hide_bars);
    zox_tst_check(!zox_tst_hide_panel, "the panel hid, only its bars should");
}
