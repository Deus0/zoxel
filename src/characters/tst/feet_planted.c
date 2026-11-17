// tst: feet_planted, the player's feet stay planted as its body shifts, stepping back under it when far (bones/ik
//  sys/feet.c, sys/leg_ik.c; XR's body plants its feet)
//  - PlantFeet on its skeleton, settled: each foot's place under its hip noted (in the body's space)
//  - shifted 5cm (a lean): both feet stay where they were in the world, the legs angled
//  - shifted 40cm more: each foot steps back under its hip, one at a time, held all the while (not let go and
//    replanted: Grounded flickers)
//  - walking fast: the legs go back to the walk's swing (the weight off)
//  - run: bash bsh/tst.sh feet_planted
double zox_tst_feet_start = 0;
int zox_tst_feet_stage = 0;
float3 zox_tst_feet_world[2];       // the feet in the world, settled
float3 zox_tst_feet_under[2];       // the feet in the body's space, settled
float zox_tst_feet_lean_moved = -1; // the feet moved in the world, leaning (planted: 0)
float zox_tst_feet_lean_angle = 1;  // the thighs' w, leaning (angled: under 1)
float zox_tst_feet_stepped = -1;    // the feet from under their hips after the steps
int zox_tst_feet_both_stepping = 0; // frames both feet stepped at once
int zox_tst_feet_steps = 0;         // frames a foot stepped
float zox_tst_feet_held = 1;        // the legs' least weight while standing (held: 1, let go they replant)
float zox_tst_feet_walk_weight = 1; // the legs' weight walking fast

static byte zox_tst_feet_legs(ecs* world, entity skeleton, ik_leg* legs) {
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    byte count = 0;
    for (int i = 0; bones && i < bones->length && count < 2; i++) {
        if (zox_valid(bones->value[i]) && zox_has(bones->value[i], ThighBone) && ik_leg_find(world, bones->value[i], &legs[count])) {
            count++;
        }
    }
    return count;
}

static inline float3 zox_tst_feet_in_body(ecs* world, entity character, float3 p) {
    return float4_rotate_float3(quaternion_inverse(zox_getv(character, Rotation3D)), float3_sub(p, zox_getv(character, Position3D)));
}

static inline void zox_tst_feet_shift(ecs* world, entity character, float x) {
    float3 p = zox_getv(character, Position3D);
    p = float3_add(p, float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { x, 0, 0 }));
    zox_setv(character, Position3D, p);
}

byte zox_tst_feet_planted_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    ik_leg legs[2];
    if (!zox_valid(skeleton) || zox_tst_feet_legs(world, skeleton, legs) < 2) {
        zox_tst_waiting = "the player's legs";
        return 0;
    }
    if (!zox_tst_feet_start) {
        zox_tst_feet_start = zox_current_time;
        zox_add(skeleton, PlantFeet);
    }
    const double t = zox_current_time - zox_tst_feet_start;
    zox_tst_progress(t * 10);
    byte stepping = 0;
    for (int i = 0; i < 2; i++) {
        stepping += zox_getv(legs[i].thigh, FootStep) > 0;
    }
    zox_tst_feet_both_stepping += stepping > 1;
    zox_tst_feet_steps += stepping > 0;
    if (zox_tst_feet_stage >= 1 && zox_tst_feet_stage <= 2) {
        zox_tst_feet_held = float_min(zox_tst_feet_held, float_min(zox_getv(legs[0].thigh, IKWeight), zox_getv(legs[1].thigh, IKWeight)));
    }
    switch (zox_tst_feet_stage) {
        case 0:
            zox_tst_waiting = "the feet planted";
            if (t > 1.5) {
                for (int i = 0; i < 2; i++) {
                    zox_tst_feet_world[i] = zox_getv(legs[i].foot, Position3D);
                    zox_tst_feet_under[i] = zox_tst_feet_in_body(world, character, zox_tst_feet_world[i]);
                }
                zox_tst_feet_shift(world, character, 0.05f);
                zox_tst_feet_stage++;
            }
            return 0;
        case 1:
            zox_tst_waiting = "leaning";
            if (t > 2.0) {
                zox_tst_feet_lean_moved = 0;
                for (int i = 0; i < 2; i++) {
                    zox_tst_feet_lean_moved = float_max(zox_tst_feet_lean_moved,
                        float3_distance(zox_getv(legs[i].foot, Position3D), zox_tst_feet_world[i]));
                    zox_tst_feet_lean_angle = float_min(zox_tst_feet_lean_angle, fabsf(zox_getv(legs[i].thigh, LocalRotation3D).w));
                }
                zox_tst_feet_shift(world, character, 0.4f);
                zox_tst_feet_stage++;
            }
            return 0;
        case 2:
            zox_tst_waiting = "the feet to step under the body";
            if (t > 3.5) {
                zox_tst_feet_stepped = 0;
                for (int i = 0; i < 2; i++) {
                    const float3 under = zox_tst_feet_in_body(world, character, zox_getv(legs[i].foot, Position3D));
                    const float dx = under.x - zox_tst_feet_under[i].x, dz = under.z - zox_tst_feet_under[i].z;
                    zox_tst_feet_stepped = float_max(zox_tst_feet_stepped, sqrtf(dx * dx + dz * dz));
                }
                zox_tst_feet_stage++;
            }
            return 0;
        case 3:
            zox_tst_waiting = "walking";
            {
                Velocity3D* velocity = zox_getm(character, Velocity3D);
                const float3 ahead = float4_rotate_float3(zox_getv(character, Rotation3D), (float3) { 0, 0, 2 });
                velocity->value.x = ahead.x;
                velocity->value.z = ahead.z;
            }
            if (t > 4.5) {
                zox_tst_feet_walk_weight = float_max(zox_getv(legs[0].thigh, IKWeight), zox_getv(legs[1].thigh, IKWeight));
                return 1;
            }
            return 0;
    }
    return 0;
}

void zox_tst_feet_planted_check(ecs* world) {
    (void) world;
    zox_log("[tst] feet_planted: leaning 5cm the feet moved [%.4f], thighs w [%.3f]; after 40cm the feet [%.4f] from under the hips, stepping [%i] frames, both at once [%i]; walking, the weight [%.2f]",
        zox_tst_feet_lean_moved, zox_tst_feet_lean_angle, zox_tst_feet_stepped, zox_tst_feet_steps, zox_tst_feet_both_stepping, zox_tst_feet_walk_weight);
    zox_tst_check(zox_tst_feet_lean_moved >= 0 && zox_tst_feet_lean_moved < 0.01f, "leaning, a planted foot moved [%f]", zox_tst_feet_lean_moved);
    zox_tst_check(zox_tst_feet_lean_angle < 0.9999f, "leaning, the legs didn't angle [%f]", zox_tst_feet_lean_angle);
    zox_log("[tst] feet_planted: standing, the legs' least weight [%.3f]", zox_tst_feet_held);
    zox_tst_check(zox_tst_feet_held > 0.95f, "standing, the feet let go [%f] (they replant, not step)", zox_tst_feet_held);
    zox_tst_check(zox_tst_feet_steps >= 4, "the feet didn't both step [%i frames]", zox_tst_feet_steps);
    zox_tst_check(zox_tst_feet_stepped >= 0 && zox_tst_feet_stepped < 0.03f, "the feet didn't step back under the body [%f]", zox_tst_feet_stepped);
    zox_tst_check(zox_tst_feet_both_stepping == 0, "both feet stepped at once [%i frames]", zox_tst_feet_both_stepping);
    zox_tst_check(zox_tst_feet_walk_weight < 0.05f, "walking, the feet stayed planted [%f]", zox_tst_feet_walk_weight);
}

void zox_tsts_feet_planted(ecs* world) {
    zox_tst_add("feet_planted", zox_tst_feet_planted_ready, zox_tst_feet_planted_check, 120);
}
