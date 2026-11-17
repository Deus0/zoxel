// tst: ik_two_bone, the arm solve's maths (fun/two_bone.c), no world
//  - over seeded random roots, bone lengths, targets and poles: the bones keep their lengths, a reachable target is
//      reached, an unreachable one is reached for (the end on the line to it), the elbow bends to the pole's side
//  - run: bash bsh/tst.sh ik_two_bone
uint zox_tst_ik_seed = 4321;

static inline float zox_tst_ik_rand() {
    uint x = zox_tst_ik_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_ik_seed = x;
    return (float) (x % 200001) / 100000.0f - 1.0f;
}

static inline float3 zox_tst_ik_rand3(float scale) {
    return (float3) { zox_tst_ik_rand() * scale, zox_tst_ik_rand() * scale, zox_tst_ik_rand() * scale };
}

byte zox_tst_ik_ready(ecs* world) {
    return 1;
}

void zox_tst_ik_two_bone_check(ecs* world) {
    float worst_length = 0, worst_reach = 0, worst_line = 0;
    int wrong_side = 0;
    for (int i = 0; i < 1000; i++) {
        const float3 root = zox_tst_ik_rand3(10);
        const float a = 0.2f + (zox_tst_ik_rand() + 1) * 0.5f;
        const float b = 0.2f + (zox_tst_ik_rand() + 1) * 0.5f;
        const float3 target = float3_add(root, zox_tst_ik_rand3(2));
        const float3 pole = zox_tst_ik_rand3(1);
        float3 elbow, end;
        ik_two_bone(root, a, b, target, pole, &elbow, &end);
        worst_length = float_max(worst_length, fabsf(float3_distance(root, elbow) - a));
        worst_length = float_max(worst_length, fabsf(float3_distance(elbow, end) - b));
        const float d = float3_distance(root, target);
        if (d > fabsf(a - b) + 0.001f && d < a + b - 0.001f) {
            worst_reach = float_max(worst_reach, float3_distance(end, target));
            // the elbow on the pole's side of the line
            const float3 dir = float3_normalize(float3_sub(target, root));
            const float3 side = float3_sub(pole, float3_scale(dir, float3_dot(pole, dir)));
            if (float3_length(side) > 0.01f && float3_dot(float3_sub(elbow, root), side) < 0) {
                wrong_side++;
            }
        } else if (d > 0.001f) {
            const float3 dir = float3_normalize(float3_sub(target, root));
            worst_line = float_max(worst_line, float3_length(float3_sub(
                float3_sub(end, root), float3_scale(dir, float3_dot(float3_sub(end, root), dir)))));
        }
    }
    zox_log("[tst] ik worst: length [%f] reach [%f] line [%f], wrong side [%i]", worst_length, worst_reach, worst_line, wrong_side);
    zox_tst_check(worst_length < 0.001f, "a bone's length changed by [%f]", worst_length);
    zox_tst_check(worst_reach < 0.001f, "a reachable target missed by [%f]", worst_reach);
    zox_tst_check(worst_line < 0.001f, "an unreachable target's reach left its line by [%f]", worst_line);
    zox_tst_check(wrong_side == 0, "[%i] elbows bent away from the pole", wrong_side);
}

void zox_tsts_ik(ecs* world) {
    zox_tst_add("ik_two_bone", zox_tst_ik_ready, zox_tst_ik_two_bone_check, 30);
}
