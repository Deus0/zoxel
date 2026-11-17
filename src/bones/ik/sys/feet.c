// Feet planted (PlantFeet on a skeleton: XR's body): each foot stays where it's planted in the world as the body
//  shifts over it (the legs angle, leg_ik.c), stepping back under the hips when it's too far from there
//  - a foot's rest: where it stands with its leg straight, under its hip, now (ik_leg_rest)
//  - too far (feet_step_distance of the leg's length, across the ground; feet_settle_distance with the body still,
//    the feet shuffle back under it after moving, a step lands where the hip was) it steps there: an arc over feet_step_time,
//    landing on the rest as it is then; one foot at a time, the farther first
//  - its height follows the rest's (the ground under the body, the hips' shift taken off), only across the ground held
//  - walking (fast) or in the air (falling, jumping) the walk's swing has the legs: the weight eases off, and back on planting the
//    feet afresh where they stand
//  - zoxp_trails: after physics moved the body, before the solve (zoxp_layouts)
#define feet_step_distance 0.3f     // of the leg's length, from its rest, across the ground
#define feet_step_time 0.25f        // seconds
#define feet_settle_distance 0.05f  // of the leg's length: still, a foot this far off its rest steps home
#define feet_still_speed 0.05f      // metres a second across the ground: the body still
#define feet_step_lift 0.15f        // of the leg's length, the step's arc
#define feet_walk_speed 0.8f        // metres a second: faster the swing has the legs
#define feet_weight_rate 10.0f
#define feet_air_speed 1.5f         // metres a second up or down: in the air, the swing has the legs
#define feet_legs_max 4

static inline float feet_smoothstep(float t) {
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return t * t * (3 - 2 * t);
}

void feet_plant_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time < 0.1f ? zox_delta_time : 0.1f;
    const float weight_t = 1.0f - expf(-feet_weight_rate * delta);
    zox_sys_begin();
    zox_sys_in(BoneLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BoneLinks, bones);
        const entity owner = zox_get_parent(world, e);
        if (!zox_valid(owner)) {
            continue;
        }
        const float3 velocity = zox_has(owner, Velocity3D) ? zox_getv(owner, Velocity3D) : float3_zero;
        // in the air by its fall or jump, not Grounded (it flickers on seams: the feet let go and replanted)
        const float speed = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        const float step_distance = speed < feet_still_speed ? feet_settle_distance : feet_step_distance;
        const byte planting = fabsf(velocity.y) < feet_air_speed && sqrtf(velocity.x * velocity.x + velocity.z * velocity.z) < feet_walk_speed;
        const skeleton_now now = skeleton_now_get(world, e);
        // shifted hips (bending, crouching) move the thighs, not where the feet stand
        const float3 shift = zox_has(e, HipsShift) ?
            float4_rotate_float3(now.now_rotation, zox_getv(e, HipsShift)) : float3_zero;
        ik_leg legs[feet_legs_max];
        float3 rests[feet_legs_max];
        float lengths[feet_legs_max];
        byte count = 0;
        byte stepping = 0;
        for (int j = 0; j < bones->length && count < feet_legs_max; j++) {
            const entity thigh = bones->value[j];
            if (!zox_valid(thigh) || !zox_has(thigh, ThighBone) || !zox_has(thigh, FootPlant) || !ik_leg_find(world, thigh, &legs[count])) {
                continue;
            }
            rests[count] = ik_leg_rest(world, &legs[count], &now);
            rests[count] = float3_sub(rests[count], shift);
            lengths[count] = ik_leg_length(world, &legs[count]);
            stepping |= zox_getv(thigh, FootStep) > 0;
            count++;
        }
        // the farthest foot past its step, if none is stepping
        int step = -1;
        float farthest = 0;
        for (byte j = 0; j < count && planting && !stepping; j++) {
            const entity thigh = legs[j].thigh;
            if (zox_getv(thigh, IKWeight) <= 0.001f) {
                continue;   // planted afresh below
            }
            const float3 plant = zox_getv(thigh, FootPlant);
            const float dx = plant.x - rests[j].x, dz = plant.z - rests[j].z;
            const float far = sqrtf(dx * dx + dz * dz) / lengths[j];
            if (far > step_distance && far > farthest) {
                farthest = far;
                step = j;
            }
        }
        for (byte j = 0; j < count; j++) {
            const entity thigh = legs[j].thigh;
            IKWeight* weight = zox_getm(thigh, IKWeight);
            FootPlant* plant = zox_getm(thigh, FootPlant);
            FootStep* foot_step = zox_getm(thigh, FootStep);
            if (!planting) {
                foot_step->value = 0;
                weight->value += (0 - weight->value) * weight_t;
                if (weight->value < 0.01f) {
                    weight->value = 0;
                }
                continue;
            }
            if (weight->value <= 0.001f) {
                plant->value = rests[j];    // planted afresh where it stands
            }
            if (j == step) {
                zox_getm(thigh, FootStepFrom)->value = plant->value;
                foot_step->value = 0.0001f;
            }
            float3 target;
            if (foot_step->value > 0) {
                foot_step->value += delta / feet_step_time;
                const float p = foot_step->value >= 1 ? 1 : foot_step->value;
                const float3 from = zox_getv(thigh, FootStepFrom);
                target = float3_add(from, float3_scale(float3_sub(rests[j], from), feet_smoothstep(p)));
                target.y = rests[j].y + sinf(p * (float) M_PI) * feet_step_lift * lengths[j];
                if (foot_step->value >= 1) {
                    foot_step->value = 0;
                    plant->value = rests[j];
                }
            } else {
                plant->value.y = rests[j].y;    // on the ground under the body
                target = plant->value;
            }
            zox_getm(thigh, IKTarget)->value = target;
            weight->value += (1 - weight->value) * weight_t;
        }
    }
    zox_sys_on_end();
} zoxd_system(feet_plant_system);
