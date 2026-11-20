// tst: touch_actions, playing with touch: a tap on an action's icon in the actionbar selects it
//  - in a game, a fake finger tap first: it switches the device mode to touchscreen (the touch ui spawns, with 4
//    buttons: pause, game menus, speak, jump, attack, no switch action buttons)
//  - a tap on the third action's icon: the character's ActionIndex is 2, nothing is picked up (the mouse's data)
//  - run: bash bsh/tst.sh touch_actions
int zox_tst_ta_stage = 0;
int zox_tst_ta_frames = 0;
byte zox_tst_ta_touch_mode = 0, zox_tst_ta_found = 0;
int zox_tst_ta_buttons = -1;
int zox_tst_ta_index = -1;
entity zox_tst_ta_held = 0;
float2 zox_tst_ta_at;    // the icon, as a finger's position (0 to 1, y from the top)

#define zox_tst_ta_target 2
#define zox_tst_ta_next(stage) { zox_tst_ta_stage = stage; zox_tst_ta_frames = 0; return 0; }

byte zox_tst_touch_actions_ready(ecs* world) {
    if (zox_tst_ta_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_ta_next(1);
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    if (!zox_valid(character) || !zox_valid(canvas) || !zox_valid(local_touchscreen)) {
        zox_tst_waiting = "the player's character, canvas and touchscreen";
        return 0;
    }
    zox_tst_ta_frames++;
    zox_tst_progress(zox_tst_ta_stage);
    if (zox_tst_ta_stage == 1) {
        // a tap on the middle (no ui there): the device mode switches to touchscreen
        zox_tst_waiting = "a tap";
        zox_tst_tp_touch(0.5f, 0.5f, zox_tst_ta_frames < 5);
        if (zox_tst_ta_frames < 20) {
            return 0;
        }
        zox_tst_ta_touch_mode = zox_getv(dbg_player, DeviceMode) == zox_device_mode_touchscreen;
        const entity touch_ui = zox_get_child_by_id(world, canvas, zox_id(MenuPlayTouch));
        zox_tst_ta_buttons = zox_valid(touch_ui) ? (int) zox_get_children_count(world, touch_ui) : -1;
        zox_tst_ta_next(2);
    }
    if (zox_tst_ta_stage == 2) {
        // the third action's icon on screen
        zox_tst_waiting = "the actionbar's icons";
        const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
        const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuActions));
        entity slots[zox_children_capacity];
        const uint length = zox_valid(actionbar) ? zox_get_children(world, actionbar, slots, zox_children_capacity) : 0;
        const entity icon = zox_valid(menu) && length > zox_tst_ta_target ?
            zox_tst_split_find_icon(world, menu, slots[zox_tst_ta_target]) : 0;
        int2 at;
        if (!icon || !zox_tst_ui_pointer_position(world, canvas, icon, &at)) {
            return zox_tst_ta_frames > 60;
        }
        // pointer space has y from the bottom, fingers from the top (apps/sdl/inputs/sys/touchscreen.c)
        const int2 size = zox_getv(canvas, LayoutSize);
        zox_tst_ta_at = (float2) { (at.x + 0.5f) / size.x, 1 - (at.y + 0.5f) / size.y };
        zox_tst_ta_found = 1;
        zox_tst_ta_next(3);
    }
    if (zox_tst_ta_stage == 3) {
        // a tap on it
        zox_tst_waiting = "a tap on the action";
        zox_tst_tp_touch(zox_tst_ta_at.x, zox_tst_ta_at.y, zox_tst_ta_frames < 5);
        if (zox_tst_ta_frames < 15) {
            return 0;
        }
        zox_tst_ta_index = zox_getv(character, ActionIndex);
        zox_tst_ta_held = zox_tst_split_mouse_data(world, canvas);
    }
    zox_touch_fake = 0;
    return 1;
}

void zox_tst_touch_actions_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_ta_touch_mode, "a tap didn't switch the device mode to touchscreen");
    zox_tst_check(zox_tst_ta_buttons == 5, "the touch ui has [%i] buttons, not 5 (pause, game menus, speak, jump, attack)", zox_tst_ta_buttons);
    zox_tst_check(zox_tst_ta_found, "no icon on screen for the actionbar's slot [%i]", zox_tst_ta_target);
    zox_tst_check(zox_tst_ta_index == zox_tst_ta_target, "tapped action [%i], the action is [%i]", zox_tst_ta_target, zox_tst_ta_index);
    zox_tst_check(!zox_tst_ta_held, "the tap picked the action up (the mouse holds it)");
}
