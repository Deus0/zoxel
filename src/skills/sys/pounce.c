// a pounce (fun/pounce.c): it squashes down, springs at what it aims at (RaycastAim, else ahead) stretched up, hits
//  the first character its ray finds close in the air (melee_hit, once), and is round again as it lands
#define pounce_crouch 0.3           // seconds squashing down
#define pounce_air 0.7              // seconds in the air at most
#define pounce_leap 5.0f            // its speed at its target
#define pounce_jump 4.5f            // its speed up
#define pounce_hit_reach 0.6f       // its ray to its target's side, its hit lands

void pounce_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PounceStart);
    zox_sys_in(PounceSkill);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Grounded);
    zox_sys_in(RaycastVoxelData);
    zox_sys_out(PounceStage);
    zox_sys_out(Scale3);
    zox_sys_out(Velocity3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PounceStart, start);
        zox_sys_i(PounceSkill, skill);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Grounded, grounded);
        zox_sys_i(RaycastVoxelData, ray);
        zox_sys_o(PounceStage, stage);
        zox_sys_o(Scale3, scale);
        zox_sys_o(Velocity3D, velocity);
        const float t = zox_current_time - start->value;
        if (stage->value == 0) {
            // squashing down, wider
            const float s = float_min(t / pounce_crouch, 1);
            scale->value = (float3) { 1 + 0.3f * s, 1 - 0.4f * s, 1 + 0.3f * s };
            if (t < pounce_crouch) {
                continue;
            }
            float3 ahead = zox_has(e, RaycastAim) ?
                float3_subtract(zox_getv(e, RaycastAim), position->value) :
                quaternion_to_normal(rotation->value);
            ahead.y = 0;
            ahead = float3_normalize(ahead);
            velocity->value = (float3) { ahead.x * pounce_leap, pounce_jump, ahead.z * pounce_leap };
            stage->value = 1;
            continue;
        }
        // in the air: stretched up, easing back round
        const float k = float_min((t - pounce_crouch) / pounce_air, 1);
        scale->value = (float3) { 1 - 0.15f * (1 - k), 1 + 0.3f * (1 - k), 1 - 0.15f * (1 - k) };
        if (stage->value == 1 && ray->result == rayhit_character && ray->distance <= pounce_hit_reach) {
            melee_hit(world, skill->value, e);
            stage->value = 2;
        }
        if (k >= 1 || (grounded->value && t > pounce_crouch + 0.2f)) {
            zox_remove(e, PounceStart);
            zox_remove(e, PounceStage);
            zox_remove(e, PounceSkill);
            zox_remove(e, Scale3);
        }
    }
    zox_sys_on_end();
} zoxd_system(pounce_system);
