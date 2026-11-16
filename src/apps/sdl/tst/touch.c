// tst: sdl_touch, a finger on the touchscreen reaches its finger zevice (touchscreen_extract_system)
//  - fake fingers (zox_touch_fake, no touchscreen needed) in place of sdl's
//  - down: a disabled finger zevice takes it (enabled, its id, pressed, at its place in pointer space: bottom left)
//  - dragged right: its position follows, its delta is to the right
//  - up: released, then disabled again (hidden, no id)
//  - run: bash bsh/tst.sh sdl_touch
int zox_tst_st_stage = 0;
int zox_tst_st_frames = 0;
entity zox_tst_st_finger = 0;
int2 zox_tst_st_down_at, zox_tst_st_expected;
byte zox_tst_st_down = 0, zox_tst_st_pressed = 0, zox_tst_st_placed = 0;
byte zox_tst_st_moved = 0;
byte zox_tst_st_released = 0, zox_tst_st_disabled = 0;

void zox_tst_st_touch(float x, float y, byte down) {
    zox_touch_fake = 1;
    zox_touch_fake_count = down ? 1 : 0;
    zox_touch_fake_fingers[0] = (zox_touch_finger) { 0, x, y };
}

// the finger zevice holding finger id 0 (zevice id 1)
entity zox_tst_st_find(ecs* world) {
    entity children[zox_children_capacity];
    const uint length = zox_get_children(world, local_touchscreen, children, zox_children_capacity);
    for (uint i = 0; i < length; i++) {
        if (zox_has(children[i], Finger) && zox_getv(children[i], ID) == 1) {
            return children[i];
        }
    }
    return 0;
}

byte zox_tst_sdl_touch_ready(ecs* world) {
    if (!zox_valid(local_touchscreen) || !zox_valid(main_app)) {
        zox_tst_waiting = "the touchscreen";
        return 0;
    }
    zox_tst_st_frames++;
    zox_tst_progress(zox_tst_st_stage);
    const int2 size = zox_getv(main_app, WindowSize);
    if (zox_tst_st_stage == 0) {
        zox_tst_waiting = "a few frames";
        if (zox_tst_st_frames < 10) {
            return 0;
        }
        zox_tst_st_touch(0.25f, 0.25f, 1);
        // pointer space: y from the bottom
        zox_tst_st_expected = (int2) { (int) (0.25f * size.x), size.y - (int) (0.25f * size.y) };
        zox_tst_st_stage = 1;
        zox_tst_st_frames = 0;
        return 0;
    }
    if (zox_tst_st_stage == 1) {
        zox_tst_waiting = "the finger down";
        zox_tst_st_finger = zox_tst_st_find(world);
        if (!zox_tst_st_finger && zox_tst_st_frames < 10) {
            return 0;
        }
        if (zox_tst_st_finger) {
            zox_tst_st_down = !zox_has(zox_tst_st_finger, Disabled);
            zox_tst_st_pressed = devices_get_pressed(zox_getv(zox_tst_st_finger, ZevicePointer));
            zox_tst_st_down_at = zox_getv(zox_tst_st_finger, ZevicePointerPosition);
            zox_tst_st_placed = abs(zox_tst_st_down_at.x - zox_tst_st_expected.x) <= 2 &&
                abs(zox_tst_st_down_at.y - zox_tst_st_expected.y) <= 2;
        }
        zox_tst_st_touch(0.5f, 0.25f, 1);
        zox_tst_st_stage = 2;
        zox_tst_st_frames = 0;
        return 0;
    }
    if (zox_tst_st_stage == 2) {
        zox_tst_waiting = "the finger dragged";
        if (zox_tst_st_frames < 2) {
            return 0;
        }
        if (zox_valid(zox_tst_st_finger)) {
            const int2 at = zox_getv(zox_tst_st_finger, ZevicePointerPosition);
            zox_tst_st_moved = abs(at.x - (int) (0.5f * size.x)) <= 2;
        }
        zox_tst_st_touch(0.5f, 0.25f, 0);
        zox_tst_st_stage = 3;
        zox_tst_st_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the finger up";
    if (zox_valid(zox_tst_st_finger)) {
        zox_tst_st_released |= devices_get_released_this_frame(zox_getv(zox_tst_st_finger, ZevicePointer));
        zox_tst_st_disabled = zox_has(zox_tst_st_finger, Disabled) && zox_getv(zox_tst_st_finger, ID) == 0;
    }
    if (!zox_tst_st_disabled && zox_tst_st_frames < 10) {
        return 0;
    }
    zox_touch_fake = 0;
    return 1;
}

void zox_tst_sdl_touch_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_st_finger != 0, "a finger down, no finger zevice took it");
    zox_tst_check(zox_tst_st_down, "a finger down, its zevice stayed disabled");
    zox_tst_check(zox_tst_st_pressed, "a finger down, its zevice isn't pressed");
    zox_tst_check(zox_tst_st_placed, "a finger down at [%ix%i], its zevice is at [%ix%i]",
        zox_tst_st_expected.x, zox_tst_st_expected.y, zox_tst_st_down_at.x, zox_tst_st_down_at.y);
    zox_tst_check(zox_tst_st_moved, "dragged right, its zevice didn't follow");
    zox_tst_check(zox_tst_st_released, "up, its zevice wasn't released");
    zox_tst_check(zox_tst_st_disabled, "up, its zevice wasn't disabled again (or kept its id)");
}
