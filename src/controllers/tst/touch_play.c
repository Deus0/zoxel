// tst: touch_play, playing with touch: virtual joysticks walk and turn the character
//  - in a game, fake fingers (zox_touch_fake): a tap first, it switches the device mode to touchscreen (the touch ui
//    spawns, MenuPlayTouch)
//  - a finger down on the left half spawns a virtual joystick there, dragged up its stick points up, held the
//    character walks forward; let go, the joystick goes and its stick is zero
//  - a finger down on the right half, dragged right: the character turns (its yaw)
//  - run: bash bsh/tst.sh touch_play
int zox_tst_tp_stage = 0;
int zox_tst_tp_frames = 0;
entity zox_tst_tp_joystick = 0;     // the first finger's virtual joystick zevice
float3 zox_tst_tp_start;
float zox_tst_tp_yaw = 0;
double zox_tst_tp_walk_time = 0;
byte zox_tst_tp_touch_mode = 0, zox_tst_tp_touch_ui = 0, zox_tst_tp_spawned = 0;
float2 zox_tst_tp_stick;
float zox_tst_tp_walked = 0, zox_tst_tp_turned = 0;
byte zox_tst_tp_gone = 0, zox_tst_tp_zeroed = 0;

void zox_tst_tp_touch(float x, float y, byte down) {
    zox_touch_fake = 1;
    zox_touch_fake_count = down ? 1 : 0;
    zox_touch_fake_fingers[0] = (zox_touch_finger) { 0, x, y };
}

// the first finger zevice's virtual joystick (inputs/ins/touchscreen.c links one a finger)
entity zox_tst_tp_find_joystick(ecs* world) {
    entity children[zox_children_capacity];
    const uint length = zox_get_children(world, local_touchscreen, children, zox_children_capacity);
    for (uint i = 0; i < length; i++) {
        const entity joystick = zox_has(children[i], Finger) ? zox_get_link(world, children[i], VirtualZeviceLink) : 0;
        if (zox_valid(joystick)) {
            return joystick;
        }
    }
    return 0;
}

#define zox_tst_tp_next(stage) { zox_tst_tp_stage = stage; zox_tst_tp_frames = 0; return 0; }

byte zox_tst_touch_play_ready(ecs* world) {
    if (zox_tst_tp_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_tp_next(1);
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    if (!zox_valid(character) || !zox_valid(canvas) || !zox_valid(local_touchscreen)) {
        zox_tst_waiting = "the player's character, canvas and touchscreen";
        return 0;
    }
    zox_tst_tp_frames++;
    zox_tst_progress(zox_tst_tp_stage);
    if (zox_tst_tp_stage == 1) {
        // a tap: the device mode switches to touchscreen
        zox_tst_waiting = "a tap";
        zox_tst_tp_touch(0.5f, 0.5f, zox_tst_tp_frames < 5);
        if (zox_tst_tp_frames < 20) {
            return 0;
        }
        zox_tst_tp_touch_mode = zox_getv(dbg_player, DeviceMode) == zox_device_mode_touchscreen;
        zox_tst_tp_touch_ui = zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuPlayTouch)));
        zox_tst_tp_joystick = zox_tst_tp_find_joystick(world);
        zox_tst_tp_next(2);
    }
    if (zox_tst_tp_stage == 2) {
        // the left half (sdl's y from the top: low on the screen), held still: its joystick spawns there
        zox_tst_waiting = "a joystick on the left";
        zox_tst_tp_touch(0.25f, 0.7f, 1);
        if (zox_tst_tp_frames < 5) {
            return 0;
        }
        zox_tst_tp_spawned = zox_valid(zox_tst_tp_joystick) && zox_valid(zox_get_link(world, zox_tst_tp_joystick, ElementLink));
        zox_tst_tp_start = zox_getv(character, Position3D);
        zox_tst_tp_walk_time = zox_current_time;
        zox_tst_tp_next(3);
    }
    if (zox_tst_tp_stage == 3) {
        // dragged up, held: it walks forward
        zox_tst_waiting = "walking";
        zox_tst_tp_touch(0.25f, 0.55f, 1);
        if (zox_tst_tp_frames == 5 && zox_valid(zox_tst_tp_joystick)) {
            zox_tst_tp_stick = zox_getv(zox_tst_tp_joystick, ZeviceStick);
        }
        // held two seconds (not frames: the test level's fps is a few times the world's)
        if (zox_current_time - zox_tst_tp_walk_time < 2.0) {
            return 0;
        }
        const float3 at = zox_getv(character, Position3D);
        zox_tst_tp_walked = sqrtf((at.x - zox_tst_tp_start.x) * (at.x - zox_tst_tp_start.x) +
            (at.z - zox_tst_tp_start.z) * (at.z - zox_tst_tp_start.z));
        zox_tst_tp_next(4);
    }
    if (zox_tst_tp_stage == 4) {
        // let go: the joystick goes, its stick back to zero
        zox_tst_waiting = "the joystick to go";
        zox_tst_tp_touch(0.25f, 0.55f, 0);
        if (zox_tst_tp_frames < 5) {
            return 0;
        }
        zox_tst_tp_gone = zox_valid(zox_tst_tp_joystick) && !zox_valid(zox_get_link(world, zox_tst_tp_joystick, ElementLink));
        zox_tst_tp_zeroed = zox_valid(zox_tst_tp_joystick) && float2_equals(zox_getv(zox_tst_tp_joystick, ZeviceStick), float2_zero);
        zox_tst_tp_next(5);
    }
    if (zox_tst_tp_stage == 5) {
        // the right half, held still a moment, then dragged right: it turns
        zox_tst_waiting = "turning";
        if (zox_tst_tp_frames == 1) {
            zox_tst_tp_yaw = quaternion_to_euler_y2(zox_getv(character, Rotation3D));
        }
        zox_tst_tp_touch(zox_tst_tp_frames < 5 ? 0.75f : 0.9f, 0.6f, 1);
        if (zox_tst_tp_frames < 40) {
            return 0;
        }
        zox_tst_tp_turned = fabsf(remainderf(quaternion_to_euler_y2(zox_getv(character, Rotation3D)) - zox_tst_tp_yaw, 2 * M_PI));
        zox_tst_tp_next(6);
    }
    zox_tst_waiting = "letting go";
    zox_tst_tp_touch(0.9f, 0.6f, 0);
    if (zox_tst_tp_frames < 5) {
        return 0;
    }
    zox_touch_fake = 0;
    return 1;
}

void zox_tst_touch_play_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] touch_play: stick [%.2f %.2f] walked [%.2f] turned [%.2f]",
        zox_tst_tp_stick.x, zox_tst_tp_stick.y, zox_tst_tp_walked, zox_tst_tp_turned);
    zox_tst_check(zox_tst_tp_touch_mode, "a tap didn't switch the device mode to touchscreen");
    zox_tst_check(zox_tst_tp_touch_ui, "in touchscreen mode, no touch ui (MenuPlayTouch)");
    zox_tst_check(zox_tst_tp_joystick != 0, "no virtual joystick zevice on the touchscreen's fingers");
    zox_tst_check(zox_tst_tp_spawned, "a finger down on the left, no joystick ui spawned");
    zox_tst_check(zox_tst_tp_stick.y > 0.3f, "dragged up, its stick [%.2f %.2f] doesn't point up", zox_tst_tp_stick.x, zox_tst_tp_stick.y);
    zox_tst_check(zox_tst_tp_walked > 1, "the left stick up, it walked [%.2f]", zox_tst_tp_walked);
    zox_tst_check(zox_tst_tp_gone, "let go, the joystick ui stayed");
    zox_tst_check(zox_tst_tp_zeroed, "let go, its stick isn't zero");
    zox_tst_check(zox_tst_tp_turned > 0.05f, "the right stick dragged, it turned [%.2f]", zox_tst_tp_turned);
}
