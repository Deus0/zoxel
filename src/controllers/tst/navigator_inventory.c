// tst: navigator_inventory, the real inventory on a gamepad: paused, RB to it, its first icon selected
//  - the tooltip: shown with its background (the label's frame, not only its text), its top right at the icon's
//      bottom right (tooltip_navigated_system)
//  - items moved round the grid (A picks up, the stick right, A places, stacks, swaps): each tooltip shows its
//      icon's item, none on an empty slot (it kept the old item's)
//  - then an item in hand, RB and LB fast round every window (it crashed switching windows)
//  - the stick down past its last row steps into the action bar (another window, element_navigation_system)
//  - run: bash bsh/tst.sh navigator_inventory
// tst: coop_navigator, the same for split screen's second player (its viewport part way along the screen: its
//  tooltips sat that far off), bash bsh/tst.sh coop_navigator
byte zox_tst_navinv_player = 0;     // which of zox_players
byte zox_tst_navinv_die = 0;        // coop_respawn_navigator: 1 dies first, 2 dying, 3 respawned
entity zox_tst_navinv_dead = 0;     // its character before

// tst: coop_respawn_navigator, the same as coop_navigator, the second player dying and respawning first (a crash
//  switching windows played after a death), bash bsh/tst.sh coop_respawn_navigator
void zox_tst_coop_respawn_navigator_setup() {
    zox_tst_coop_level_setup();
    zox_tst_navinv_player = 1;
    zox_tst_navinv_die = 1;
}

void zox_tst_coop_navigator_setup() {
    zox_tst_coop_level_setup();
    zox_tst_navinv_player = 1;
}
int2 zox_tst_navinv_tip_off = { 999, 999 };
byte zox_tst_navinv_tip_shown = 0;
byte zox_tst_navinv_tip_background = 0;
byte zox_tst_navinv_opened = 0;
byte zox_tst_navinv_actionbar = 0;
byte zox_tst_navinv_switched = 0;    // switched windows fast and lived
char zox_tst_navinv_last[64] = "";

int zox_tst_navinv_moves = 0;
int zox_tst_navinv_tips = 0;            // tooltips looked at, every step
int zox_tst_navinv_tips_bare = 0;       // their text without the background
int zox_tst_navinv_tips_off = 0;        // not under their icon
int zox_tst_navinv_tips_missing = 0;
int zox_tst_navinv_tips_stale = 0;     // showing another item, or one on an empty slot    // a ui with a tooltip, none shown

// the selection's tooltip: shown, with its background, under it
static inline void zox_tst_navinv_tip(ecs* world, entity canvas) {
    const entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
    const entity icon = zox_getv(zox_tst_pad, EntityTarget);
    CanvasViewport viewport;
    if (!zox_valid(tooltip) || !zox_valid(icon) || !zox_has(icon, Tooltipper) || !get_canvas_viewport(world, canvas, &viewport)) {
        return;
    }
    // an empty slot has none
    const entity slot = zox_get_link(world, icon, SlotLink);
    if (!zox_valid(slot) || !zox_is_linked_any(world, slot, DataLink)) {
        if (zox_get(tooltip, TextData)->length) {
            zox_tst_navinv_tips_stale++;
            zox_log("[tst] step [%i] [%s] is empty, its tooltip still shows", zox_tst_navinv_moves, zox_get_name(icon));
        }
        return;
    }
    zox_tst_navinv_tips++;
    const TextData* text = zox_get(tooltip, TextData);
    if (!text->length) {
        zox_tst_navinv_tips_missing++;
        zox_log("[tst] [%s] has no tooltip, slot [%lu] data [%lu]", zox_get_name(icon), slot,
            zox_valid(slot) ? zox_get_link(world, slot, DataLink) : 0);
        return;
    }
    const byte background = !zox_getv(tooltip, RenderDisabled) && zox_getv(tooltip, MeshGPULink).x;
    const int4 t = get_element_pointer_bounds(world, &viewport, tooltip, zox_getv(tooltip, Position2), zox_getv(tooltip, LayoutSize));
    const int4 b = get_element_pointer_bounds(world, &viewport, icon, zox_getv(icon, Position2), zox_getv(icon, LayoutSize));
    const int2 off = { t.y - b.y, t.w - b.z };
    // under it, or moved right or above to keep on its canvas, and on it
    const byte on_canvas = t.x >= viewport.position.x - 1 && t.z >= viewport.position.y - 1 &&
        t.y <= viewport.position.x + viewport.size.x + 1 && t.w <= viewport.position.y + viewport.size.y + 1;
    const byte kept_on = (t.x - viewport.position.x) <= 1 || t.z > b.w;
    const byte under = on_canvas && ((abs(off.x) <= 3 && off.y <= 0 && off.y >= -8) || kept_on);
    char* shown = get_zext_text(text);
    char expected[128];
    item_icon_tooltip_text(world, icon, expected, sizeof(expected));
    if (strcmp(shown, expected)) {
        zox_tst_navinv_tips_stale++;
        zox_log("[tst] step [%i] [%s]'s tooltip isn't its item's", zox_tst_navinv_moves, zox_get_name(icon));
    }
    zox_log("[tst] step [%i] [%s] tooltip [%s] size [%ix%i] disabled [%i] gpu [%i]", zox_tst_navinv_moves,
        zox_get_name(icon), shown, zox_getv(tooltip, LayoutSize).x, zox_getv(tooltip, LayoutSize).y,
        zox_getv(tooltip, RenderDisabled), zox_getv(tooltip, MeshGPULink).x);
    free(shown);
    zox_tst_navinv_tips_bare += !background;
    zox_tst_navinv_tips_off += !under;
    if (!background || !under) {
        zox_log("[tst] [%s]'s tooltip: background [%i], from its bottom right [%i %i]", zox_get_name(icon), background,
            off.x, off.y);
    }
}

// an item in every empty slot of the character's inventory and action bar (the test level's items are random, an empty
//  slot has no tooltip)
static inline void zox_tst_navinv_fill(ecs* world, entity character) {
    const entity cookie = zox_tst_held_cookie(world, zox_get_link(world, character, RealmLink));
    const entity managers[2] = { zox_get_child_by_id(world, character, zox_id(Inventory)),
        zox_get_child_by_id(world, character, zox_id(Actionbar)) };
    for (int m = 0; m < 2 && zox_valid(cookie); m++) {
        // each slot once (the links are deferred, an empty slot stays empty until the frame ends)
        entity slots[zox_children_capacity];
        const uint count = zox_valid(managers[m]) ? zox_get_children(world, managers[m], slots, zox_children_capacity) : 0;
        for (uint i = 0; i < count; i++) {
            if (!zox_get_link(world, slots[i], DataLink)) {
                zox_set_link(world, slots[i], DataLink, spawn_item_pickedup(world, cookie, character, 1));
                zox_add(slots[i], DataDirty);
            }
        }
    }
}

static inline entity zox_tst_navinv_target_window(ecs* world) {
    const entity target = zox_valid(zox_tst_pad) ? zox_getv(zox_tst_pad, EntityTarget) : 0;
    return zox_valid(target) ? zox_get_parent_by_id(world, target, zox_id(Window)) : 0;
}

byte zox_tst_navigator_inventory_ready(ecs* world) {
    if (zox_tst_nav2_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        if (zox_tst_navinv_player >= players_playing ||
            !zox_valid(zox_get_link(world, zox_players[zox_tst_navinv_player], CharacterLink))) {
            zox_tst_waiting = "the player's character";
            return 0;
        }
        zox_tst_pad = zox_tst_spawn_pad(world, zox_players[zox_tst_navinv_player]);
        zox_tst_navinv_fill(world, zox_get_link(world, zox_players[zox_tst_navinv_player], CharacterLink));
        zox_tst_nav2_next();
        return 0;
    }
    const entity player = zox_players[zox_tst_navinv_player];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    if (zox_tst_navinv_die == 1 || zox_tst_navinv_die == 2) {
        const entity character = zox_get_link(world, player, CharacterLink);
        zox_tst_progress(9000 + zox_tst_navinv_die * 100 + (int) zox_current_time);
        if (zox_tst_navinv_die == 1) {
            zox_tst_waiting = "the player killed";
            const entity health = zox_valid(character) ? zox_get_child_by_id(world, character, zox_id(StatHealth)) : 0;
            if (zox_valid(health) && zox_getv(player, PlayerState) == zox_player_state_playing) {
                zox_setv(health, StatValue, 0);
                zox_tst_navinv_dead = character;
                zox_tst_navinv_die = 2;
            }
            return 0;
        }
        zox_tst_waiting = "the player to respawn";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || !zox_valid(character) ||
            character == zox_tst_navinv_dead || zox_has(character, Dead)) {
            return 0;
        }
        zox_log("[tst] respawned as [%s]", zox_get_name(character));
        zox_tst_navinv_fill(world, character);
        zox_tst_navinv_die = 3;
        zox_tst_nav2_frames = 0;
        return 0;
    }
    const int f = ++zox_tst_nav2_frames;
    zox_tst_progress(zox_tst_nav2_stage * 1000 + f);
    memset(zox_tst_pad_held, 0, sizeof(zox_tst_pad_held));
    zox_tst_pad_stick = float2_zero;
    static int moded = 0;
    switch (zox_tst_nav2_stage) {
        case 1:
            zox_tst_waiting = "the gamepad's mode, then start pausing";
            if (!moded) {
                zox_tst_pad_held[zox_btn_y] = (f % 20) < 4;
                if (zox_getv(player, DeviceMode) == zox_device_mode_gamepad) {
                    moded = f;
                }
                return f > 600;
            }
            zox_tst_pad_held[zox_btn_start] = f >= moded + 20 && f < moded + 24;
            if (f > moded + 60 && zox_getv(player, PlayerState) == zox_player_state_paused) {
                zox_tst_nav2_next();
            }
            return f > 600;
        case 2:
            zox_tst_waiting = "RB to the inventory";
            {
                const int at = (f - 1) % 40;
                zox_tst_pad_held[zox_btn_rb] = at >= 2 && at < 6;
                const entity window = zox_tst_navinv_target_window(world);
                if (at == 30 && zox_valid(window) && zox_has(window, MenuItems)) {
                    zox_tst_navinv_opened = 1;
                    zox_tst_nav2_next();
                }
                return f > 40 * 12;
            }
        case 3:
            zox_tst_waiting = "the inventory icon's tooltip";
            if (f == 36 && getenv("zox_tst_shot")) {
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("navigator.ppm"));
            }
            if (f == 40) {
                const entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
                const entity icon = zox_getv(zox_tst_pad, EntityTarget);
                CanvasViewport viewport;
                zox_tst_navinv_tip(world, canvas);
                if (zox_valid(tooltip) && zox_valid(icon) && get_canvas_viewport(world, canvas, &viewport)) {
                    const TextData* text = zox_get(tooltip, TextData);
                    zox_tst_navinv_tip_shown = text->length > 0;
                    zox_tst_navinv_tip_background = !zox_getv(tooltip, RenderDisabled) &&
                        zox_getv(tooltip, LayoutSize).x > 0 && zox_getv(tooltip, MeshGPULink).x;
                    const int4 t = get_element_pointer_bounds(world, &viewport, tooltip, zox_getv(tooltip, Position2), zox_getv(tooltip, LayoutSize));
                    const int4 b = get_element_pointer_bounds(world, &viewport, icon, zox_getv(icon, Position2), zox_getv(icon, LayoutSize));
                    zox_tst_navinv_tip_off = (int2) { t.y - b.y, t.w - b.z };
                    zox_log("[tst] icon [%s] bounds [%i %i %i %i], tooltip [%i %i %i %i] disabled [%i] text [%i]",
                        zox_get_name(icon), b.x, b.y, b.z, b.w, t.x, t.y, t.z, t.w, zox_getv(tooltip, RenderDisabled),
                        text->length);
                }
                zox_tst_nav2_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "items moved round the grid: A picks up, the stick right, A places";
            {
                // 8 steps of 30 frames: A, right, right, A, A, right, A, right
                static const char steps[] = "ArrAAArAr";
                const int step = (f - 1) / 30, at = (f - 1) % 30;
                if (step >= (int) sizeof(steps) - 1) {
                    zox_tst_nav2_next();
                    return 0;
                }
                if (steps[step] == 'A') {
                    zox_tst_pad_held[zox_btn_a] = at >= 2 && at < 6;
                } else {
                    zox_tst_pad_stick = at >= 2 && at < 8 ? (float2) { -1, 0 } : float2_zero;
                }
                if (at == 25) {
                    zox_tst_navinv_moves++;
                    zox_tst_navinv_tip(world, canvas);
                }
                return 0;
            }
        case 5:
            zox_tst_waiting = "the stick down into the action bar";
            {
                const int at = (f - 1) % 30;
                zox_tst_pad_stick = at < 6 ? (float2) { 0, -1 } : float2_zero;
                const entity window = zox_tst_navinv_target_window(world);
                if (at == 25) {
                    zox_tst_navinv_tip(world, canvas);
                    snprintf(zox_tst_navinv_last, sizeof(zox_tst_navinv_last), "%s",
                        zox_valid(window) ? zox_get_name(window) : "none");
                    if (zox_valid(window) && zox_has(window, MenuActions)) {
                        zox_tst_navinv_actionbar = 1;
                        zox_tst_nav2_next();
                        return 0;
                    }
                }
                return f > 30 * 10;
            }
        case 6:
            zox_tst_waiting = "the stick along the action bar";
            {
                const int at = (f - 1) % 30;
                zox_tst_pad_stick = at < 6 ? (float2) { -1, 0 } : float2_zero;
                if (at == 25) {
                    zox_tst_navinv_tip(world, canvas);
                }
                if (f >= 30 * 5) {
                    zox_tst_nav2_next();
                }
                return 0;
            }
        case 7:
            // an item in hand (A on an action bar icon), then RB and LB fast round every window, twice: switching
            //  windows crashed playing
            zox_tst_waiting = "switching windows fast, an item in hand";
            zox_tst_pad_held[zox_btn_a] = f >= 2 && f < 6;
            if (f > 20) {
                const int at = (f - 20) % 8;
                const byte rb = ((f - 20) / 8) % 6 < 4;   // four right, two left
                zox_tst_pad_held[rb ? zox_btn_rb : zox_btn_lb] = at < 3;
            }
            if (f > 20 + 8 * 6 * 4) {
                zox_tst_navinv_switched = 1;
                return 1;
            }
            return 0;
    }
    return 0;
}

void zox_tst_navigator_inventory_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_navinv_opened, "RB never got the selection into the inventory");
    zox_tst_check(zox_tst_navinv_tip_shown, "the inventory icon has no tooltip text");
    zox_tst_check(zox_tst_navinv_tip_background, "the tooltip's text shows without its background");
    zox_log("[tst] the tooltip's top right from the icon's bottom right [%i %i]", zox_tst_navinv_tip_off.x, zox_tst_navinv_tip_off.y);
    zox_log("[tst] [%i] tooltips looked at: [%i] missing, [%i] without their background, [%i] not under their ui",
        zox_tst_navinv_tips, zox_tst_navinv_tips_missing, zox_tst_navinv_tips_bare, zox_tst_navinv_tips_off);
    zox_tst_check(zox_tst_navinv_tips >= 2, "only [%i] tooltips looked at", zox_tst_navinv_tips);
    zox_tst_check(!zox_tst_navinv_tips_missing, "[%i] uis navigated to showed no tooltip", zox_tst_navinv_tips_missing);
    zox_tst_check(!zox_tst_navinv_tips_bare, "[%i] tooltips showed their text without the background", zox_tst_navinv_tips_bare);
    zox_tst_check(!zox_tst_navinv_tips_stale, "[%i] tooltips showed another item than their icon's (or one on an empty slot)",
        zox_tst_navinv_tips_stale);
    zox_tst_check(!zox_tst_navinv_tips_off, "[%i] tooltips weren't under their ui", zox_tst_navinv_tips_off);
    zox_tst_check(zox_tst_navinv_switched, "never finished switching windows fast");
    zox_tst_check(zox_tst_navinv_actionbar, "the stick down from the inventory never reached the action bar, last in [%s]",
        zox_tst_navinv_last);
}
