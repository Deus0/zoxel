// a tentacle (Tentacle skill, a slime's): a whip of particles from its user's body, spiking out at what it aims at
//  (RaycastAim, else ahead), piercing its attacker if they're still on its line, then back into its body
//  - its tip set as it starts, so a step aside dodges it; it bulges in its body first (the wind up), a tell
//  - its particles its user's children (TentacleBit, their place along it), laid along it each frame
#define tentacle_bits 30
#define tentacle_windup 0.2         // seconds bulging in its body
#define tentacle_out 0.12           // spiking out
#define tentacle_hold 0.12
#define tentacle_back 0.3           // drawn back in
#define tentacle_time (tentacle_windup + tentacle_out + tentacle_hold + tentacle_back)
#define tentacle_pierce 0.55f       // its line to its attacker's centre, its hit lands

void tentacle_activate_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(SkillRange);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, active);
        zox_sys_i(SkillRange, range);
        if (active->value != zox_dirty_active) {
            continue;
        }
        const entity user = zox_get_parent(world, e);
        if (!zox_valid(user) || zox_has(user, Dead) || zox_has(user, TentacleStart) || !skill_pay_cost(world, e, user)) {
            continue;
        }
        const float3 position = zox_getv(user, Position3D);
        float3 ahead = zox_has(user, RaycastAim) ?
            float3_subtract(zox_getv(user, RaycastAim), position) :
            quaternion_to_normal(zox_getv(user, Rotation3D));
        const float length = float_min(float3_length(ahead) + 0.3f, range->value);
        const float3 tip = float3_add(position, float3_scale(float3_normalize(ahead), length));
        zox_set(user, TentacleStart, { zox_current_time });
        zox_set(user, TentacleTip, { tip });
        zox_set(user, TentacleSkill, { e });
        zox_set(user, TentacleHit, { 0 });
        const color c = zox_has(e, Color) ? zox_getv(e, Color) : (color) { 110, 210, 120, 255 };
        for (int j = 0; j < tentacle_bits; j++) {
            const entity bit = spawn_particle3(world, position, float3_zero, c);
            zox_setv(bit, Gravity3D, float3_zero);
            zox_setv(bit, DestroyInTime, tentacle_time + 0.05f);
            zox_set(bit, TentacleBit, { (float) j / (tentacle_bits - 1) });
            zox_set_parent_non_fragment(world, bit, user);
        }
    }
    zox_sys_on_end();
} zoxd_system(tentacle_activate_system);

// its reach out of its whole length, by time: 0 bulging, out, held, back
static inline float tentacle_extent(float t) {
    if (t < tentacle_windup) {
        return 0;
    }
    t -= tentacle_windup;
    if (t < tentacle_out) {
        const float k = t / tentacle_out;
        return 1 - (1 - k) * (1 - k);
    }
    t -= tentacle_out;
    if (t < tentacle_hold) {
        return 1;
    }
    t -= tentacle_hold;
    return float_max(0, 1 - t / tentacle_back);
}

// the distance from a point to a line from a to b
static inline float tentacle_line_distance(float3 p, float3 a, float3 b) {
    const float3 ab = float3_subtract(b, a);
    const float l2 = float3_dot(ab, ab);
    const float k = l2 > 0 ? float_min(1, float_max(0, float3_dot(float3_subtract(p, a), ab) / l2)) : 0;
    return float3_distance(p, float3_add(a, float3_scale(ab, k)));
}

void tentacle_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TentacleStart);
    zox_sys_in(TentacleTip);
    zox_sys_in(TentacleSkill);
    zox_sys_in(Position3D);
    zox_sys_out(TentacleHit);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TentacleStart, start);
        zox_sys_i(TentacleTip, tip);
        zox_sys_i(TentacleSkill, skill);
        zox_sys_i(Position3D, position);
        zox_sys_o(TentacleHit, hit);
        const float t = zox_current_time - start->value;
        const float extent = tentacle_extent(t);
        const float3 from = position->value;
        const float3 along = float3_subtract(tip->value, from);
        const float3 side = float3_normalize(float3_cross(along, float3_up));
        // its particles along it, a wave down it; bulging round its body before it spikes out
        entity bits[tentacle_bits];
        const uint count = zox_get_children_by_id(world, e, bits, tentacle_bits, zox_id(TentacleBit));
        for (uint j = 0; j < count; j++) {
            const float u = zox_getv(bits[j], TentacleBit);
            float3 p;
            if (t < tentacle_windup) {
                const float a = u * 12.566f + t * 20;
                const float r = 0.12f + 0.1f * (t / tentacle_windup);
                p = float3_add(from, (float3) { cosf(a) * r, 0.1f + sinf(u * 9.42f) * 0.08f, sinf(a) * r });
            } else {
                const float wave = sinf(u * 7 - t * 30) * 0.1f * u * (1 - u) * 4 * extent;
                p = float3_add(float3_add(from, float3_scale(along, u * extent)), float3_scale(side, wave));
            }
            zox_setv(bits[j], Position3D, p);
        }
        // fully out: it pierces its attacker if they're on its line
        if (!hit->value && t >= tentacle_windup + tentacle_out) {
            hit->value = 1;
            const entity target = zox_get_link(world, e, AttackerLink);
            if (zox_valid(target) && zox_has(target, Character3) && !zox_has(target, Dead) && zox_valid(skill->value) &&
                tentacle_line_distance(zox_getv(target, Position3D), from, tip->value) < tentacle_pierce) {
                const float damage = get_skill_damage(world, skill->value, e,
                    zox_getv(skill->value, SkillDamage), zox_getv(skill->value, SkillDamageMax));
                damage_character(world, e, target, skill->value, damage, float3_normalize(along));
            }
        }
        if (t >= tentacle_time) {
            zox_remove(e, TentacleStart);
            zox_remove(e, TentacleTip);
            zox_remove(e, TentacleSkill);
            zox_remove(e, TentacleHit);
        }
    }
    zox_sys_on_end();
} zoxd_system(tentacle_system);
