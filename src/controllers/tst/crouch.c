// tst: crouch, a gamepad's left stick pushed in crouches the player, again stands (controllers3/sys/movement.c)
//  - on the test level, a gamepad (zox_tst_spawn_pad, with a left stick push) the player's, its buttons pressed as a
//    real one's are; the left control key holds a crouch (SDL's keyboard state can't be held from a test)
//  - L3: the skeleton's CrouchState toggled on, eased crouched (CrouchBlend), the eyes lower (the head bone, the first
//    person camera on it), the knees bent; walking ahead (the stick) crouched is slower than standing; L3 again stands
//  - run: bash bsh/tst.sh crouch
#define zox_tst_crouch_settle 0.8       // seconds for a crouch (or a stand, or a walk's speed) to settle
int zox_tst_crouch_stage = 0;
double zox_tst_crouch_time = 0;
float zox_tst_crouch_head_standing = 0;     // the head above the body's place
float zox_tst_crouch_head_crouched = 0;
float zox_tst_crouch_blend = 0;
float zox_tst_crouch_knee = 0;              // degrees, crouched
float zox_tst_crouch_speed_walking = 0;     // across the ground, crouched and standing
float zox_tst_crouch_speed_crouched = 0;
byte zox_tst_crouch_stood = 0;

static inline float zox_tst_crouch_head(ecs* world, entity character) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    return zox_valid(head) ? zox_getv(head, Position3D).y - zox_getv(skeleton, Position3D).y : 0;
}

static inline float zox_tst_crouch_ground_speed(ecs* world, entity character) {
    const float3 v = zox_getv(character, Velocity3D);
    return sqrtf(v.x * v.x + v.z * v.z);
}

static inline void zox_tst_crouch_next() {
    zox_tst_crouch_stage++;
    zox_tst_crouch_time = zox_current_time;
}

byte zox_tst_crouch_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    if (!zox_valid(skeleton) || !zox_has(skeleton, CrouchState)) {
        zox_tst_waiting = "the player's skeleton";
        return 0;
    }
    const double t = zox_current_time - zox_tst_crouch_time;
    zox_tst_progress(zox_tst_crouch_stage * 100 + t * 10);
    zox_tst_pad_held[zox_btn_left_stick_push] = 0;
    switch (zox_tst_crouch_stage) {
        case 0: {
            zox_tst_waiting = "a gamepad";
            const entity player = dbg_player;
            zox_tst_pad = zox_tst_spawn_pad(world, player);
            spawn_device_button(world, prefab_zevice_button, zox_tst_pad, zox_btn_left_stick_push, 6);
            zox_tst_crouch_next();
            return 0;
        }
        case 1:
            // the stick touched, so the gamepad's the player's device (its first input switches to it, unseen)
            zox_tst_waiting = "standing";
            zox_tst_pad_stick = (float2) { 0, t < 0.1 ? 0.05f : 0 };
            if (t < zox_tst_crouch_settle) {
                return 0;
            }
            zox_tst_crouch_head_standing = zox_tst_crouch_head(world, character);
            zox_tst_crouch_next();
            return 0;
        case 2:
            // L3 pressed a few frames
            zox_tst_waiting = "crouching";
            zox_tst_pad_held[zox_btn_left_stick_push] = t < 0.1;
            if (t < zox_tst_crouch_settle) {
                return 0;
            }
            zox_tst_crouch_head_crouched = zox_tst_crouch_head(world, character);
            zox_tst_crouch_blend = zox_getv(skeleton, CrouchBlend);
            {
                const BoneLinks* bones = zox_get(skeleton, BoneLinks);
                for (int i = 0; i < bones->length; i++) {
                    if (zox_valid(bones->value[i]) && zox_has(bones->value[i], ThighBone)) {
                        const entity shin = zox_get_child_by_id(world, bones->value[i], zox_id(Bone));
                        if (zox_valid(shin)) {
                            zox_tst_crouch_knee = quaternion_angle(zox_getv(shin, LocalRotation3D)) * radians_to_degrees;
                        }
                        break;
                    }
                }
            }
            zox_tst_crouch_next();
            return 0;
        case 3:
            // walking ahead crouched
            zox_tst_waiting = "walking crouched";
            zox_tst_pad_stick = (float2) { 0, 1 };
            if (t < zox_tst_crouch_settle) {
                return 0;
            }
            zox_tst_crouch_speed_crouched = zox_tst_crouch_ground_speed(world, character);
            zox_tst_pad_stick = float2_zero;
            zox_tst_crouch_next();
            return 0;
        case 4:
            // L3 again: standing
            zox_tst_waiting = "standing up";
            zox_tst_pad_held[zox_btn_left_stick_push] = t < 0.1;
            if (t < zox_tst_crouch_settle) {
                return 0;
            }
            zox_tst_crouch_stood = !zox_getv(skeleton, CrouchState) && zox_getv(skeleton, CrouchBlend) < 0.05f;
            zox_tst_crouch_next();
            return 0;
        case 5:
            zox_tst_waiting = "walking standing";
            zox_tst_pad_stick = (float2) { 0, 1 };
            if (t < zox_tst_crouch_settle) {
                return 0;
            }
            zox_tst_crouch_speed_walking = zox_tst_crouch_ground_speed(world, character);
            zox_tst_pad_stick = float2_zero;
            return 1;
    }
    return 1;
}

void zox_tst_crouch_check(ecs* world) {
    (void) world;
    zox_tst_pad_stick = float2_zero;
    const float drop = zox_tst_crouch_head_standing - zox_tst_crouch_head_crouched;
    zox_log("[tst] crouch: the head [%.3f] standing, [%.3f] crouched (down [%.3f]), crouch blend [%.2f], a knee [%.1f] degrees",
        zox_tst_crouch_head_standing, zox_tst_crouch_head_crouched, drop, zox_tst_crouch_blend, zox_tst_crouch_knee);
    zox_log("[tst] crouch: walking [%.3f] standing, [%.3f] crouched", zox_tst_crouch_speed_walking, zox_tst_crouch_speed_crouched);
    zox_tst_check(zox_tst_crouch_blend > 0.9f, "L3 didn't crouch [%f]", zox_tst_crouch_blend);
    zox_tst_check(drop > 0.08f, "crouched, the head went down [%f]", drop);
    zox_tst_check(zox_tst_crouch_knee > 60, "crouched, the knees bent [%f] degrees", zox_tst_crouch_knee);
    zox_tst_check(zox_tst_crouch_speed_crouched > 0.1f && zox_tst_crouch_speed_crouched < zox_tst_crouch_speed_walking * 0.6f,
        "crouched it walked [%f], standing [%f]", zox_tst_crouch_speed_crouched, zox_tst_crouch_speed_walking);
    zox_tst_check(zox_tst_crouch_stood, "L3 again didn't stand");
}

void zox_tsts_crouch(ecs* world) {
    zox_tst_add("crouch", zox_tst_crouch_ready, zox_tst_crouch_check, 90);
    zox_tst_set_setup("crouch", zox_tst_level_setup, 0);
}
