// tst: statbars_hide, the player's stat bars hide when nothing changes, pop back in when hurt (AutoHide, ui_poke)
//  - its stats filled (nothing regenerates), waits for them to hide (statbars_hide_time idle), shrunk to nothing
//  - hurts the player, they show again at full size
//  - run: bash bsh/tst.sh statbars_hide
byte zox_tst_hide_stage = 0;
double zox_tst_hide_time = 0;
byte zox_tst_hide_hid = 0;          // hidden and shrunk once idle
float zox_tst_hide_scale = -1;      // its scale once shown again

static inline entity zox_tst_statbars(ecs* world) {
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    return zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(StatBars)) : 0;
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
    const entity bars = zox_tst_statbars(world);
    if (!zox_valid(bars)) {
        return 1;
    }
    zox_tst_progress(zox_tst_hide_stage);
    if (zox_tst_hide_stage == 1) {
        zox_tst_waiting = "the stat bars to hide";
        if (zox_has(bars, AutoHidden) && zox_getv(bars, LocalScale1) < 0.01f) {
            zox_tst_hide_hid = 1;
            // hurt: its health changes, its bar pokes them
            const entity character = zox_get_link(world, dbg_player, CharacterLink);
            const entity health = zox_get_child_by_id(world, character, zox_id(StatHealth));
            zox_setv(health, StatValue, zox_getv(health, StatValue) - 1);
            zox_tst_hide_stage = 2;
            zox_tst_hide_time = zox_current_time;
        }
        return zox_current_time - zox_tst_hide_time > statbars_hide_time + 10;
    }
    zox_tst_waiting = "the stat bars to show";
    if (zox_current_time - zox_tst_hide_time < 1) {
        return 0;
    }
    zox_tst_hide_scale = zox_has(bars, AutoHidden) ? 0 : zox_getv(bars, LocalScale1);
    return 1;
}

void zox_tst_statbars_hide_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_hide_hid, "the stat bars didn't hide after [%.0f] seconds idle", statbars_hide_time);
    zox_tst_check(fabsf(zox_tst_hide_scale - 1) < 0.01f, "hurt, the stat bars came back at [%.2f], not 1", zox_tst_hide_scale);
}
