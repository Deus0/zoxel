// tst: action_wheel, the mouse wheel switches the player's action only while the mouse is locked
//  - starts in a game (--autostart), playing with the mouse locked
//  - a wheel notch switches the character's ActionIndex
//  - with the mouse unlocked (i.e. free roam, dialogue, an open window), a notch doesn't,
//      the wheel scrolls ui then instead (see scrollview_wheel_system)
//  - run: bash bsh/tst.sh action_wheel
#define zox_tst_wheel_hold_frames 4
int zox_tst_wheel_stage = 0;
int zox_tst_wheel_frames = 0;
int zox_tst_wheel_action = 0;

static inline int zox_tst_wheel_action_index(ecs* world, entity player) {
    const entity character = zox_get_link(world, player, CharacterLink);
    return zox_valid(character) && zox_has(character, ActionIndex) ? zox_getv(character, ActionIndex) : -1;
}

static inline void zox_tst_wheel_set_lock(ecs* world, entity player, byte lock) {
    const entity mouse = zox_get_child_by_id(world, player, zox_id(Mouse));
    if (zox_valid(mouse)) {
        zox_setv(mouse, MouseLock, lock);
    }
}

byte zox_tst_action_wheel_ready(ecs* world) {
    const entity player = dbg_player;
    if (!zox_valid(player)) {
        zox_tst_waiting = "the player";
        return 0;
    }
    zox_tst_wheel_frames++;
    const int action = zox_tst_wheel_action_index(world, player);
    switch (zox_tst_wheel_stage) {
        case 0:
            zox_tst_waiting = "playing, with a character";
            if (zox_getv(player, PlayerState) != zox_player_state_playing || action < 0) {
                zox_tst_wheel_frames = 0;
                return 0;
            }
            // settles, so the play begin mouse lock has been set
            if (zox_tst_wheel_frames < 10) {
                return 0;
            }
            zox_tst_check(zox_tst_mouse_lock(world, player) == 1, "playing: mouse lock is [%i], expected locked", zox_tst_mouse_lock(world, player));
            zox_tst_wheel_action = action;
            zox_tst_mouse((int2) { 100, 100 }, 0);
            break;
        case 1:
        case 3:
            // one notch down, for a frame
            zox_tst_waiting = "a wheel notch";
            zox_tst_mouse_wheel(zox_tst_wheel_frames == 1 ? -1 : 0);
            if (zox_tst_wheel_frames < zox_tst_wheel_hold_frames) {
                return 0;
            }
            if (zox_tst_wheel_stage == 1) {
                zox_tst_check(action != zox_tst_wheel_action, "mouse locked: a wheel notch didn't switch the action from [%i]", zox_tst_wheel_action);
            } else {
                zox_tst_check(action == zox_tst_wheel_action, "mouse unlocked: a wheel notch switched the action [%i] to [%i]", zox_tst_wheel_action, action);
                zox_tst_wheel_set_lock(world, player, 1);
                zox_tst_mouse_end();
                return 1;
            }
            break;
        case 2:
            zox_tst_waiting = "unlocking the mouse";
            zox_tst_wheel_set_lock(world, player, 0);
            zox_tst_wheel_action = action;
            if (zox_tst_wheel_frames < zox_tst_wheel_hold_frames) {
                return 0;
            }
            break;
    }
    zox_tst_wheel_stage++;
    zox_tst_wheel_frames = 0;
    return 0;
}

void zox_tst_action_wheel_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
