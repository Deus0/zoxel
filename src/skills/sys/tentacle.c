// a tentacle (Tentacle skill, a slime's): a whip of particles from its user's body, spiking out at what it aims at
//  (RaycastAim, else ahead), hitting its attacker if one of its particles touches them, then back into its body
//  - its tip set as it starts, so a step aside dodges it; it bulges in its body first (the wind up), a tell
//  - it stops short of the first block on its way (tentacle_clear_length): it hit through walls before
//  - a SkillUsing (its tip the SkillTarget, SkillHit its hit tried)
//  - its particles its user's children (Particle3D), laid along it each frame in their order: rings of strands down
//    it, thick at its body and thin at its tip, twisting round; it arches up with a wave running down it, its tip
//    curling as it's drawn back; dark at its root, light at its tip
#define tentacle_segments 26
#define tentacle_strands 3
#define tentacle_bits (tentacle_segments * tentacle_strands)
#define tentacle_thick 8.0f         // its particles' size in voxels at its body, to its tip
#define tentacle_thin 4.0f
#define tentacle_root 0.14f         // its thickness at its body (a ring's radius), thinning to its tip
#define tentacle_windup 0.2         // seconds bulging in its body
#define tentacle_out 0.12           // spiking out
#define tentacle_hold 0.18
#define tentacle_back 0.35          // drawn back in
#define tentacle_time (tentacle_windup + tentacle_out + tentacle_hold + tentacle_back)
#define tentacle_touch 0.06f        // a particle this near its attacker's bounds touches them

// how far a line goes from a, along a direction, before a solid block (sampled every quarter block), at most length
static inline float tentacle_clear_length(ecs* world, entity terrain, float3 a, float3 direction, float length) {
    if (!zox_valid(terrain)) {
        return length;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const float step = 0.25f * scale;
    for (float d = step; d < length; d += step) {
        const float3 p = float3_add(a, float3_scale(direction, d));
        if (terrain_voxel_at(world, terrain, real_position_to_block_position(p, scale)) > 0) {
            return float_max(0, d - step);
        }
    }
    return length;
}

// a point inside a body's bounds (Bounds3D, its half size) and a margin
static inline byte tentacle_touches(float3 p, float3 centre, float3 bounds) {
    const float3 d = float3_subtract(p, centre);
    return fabsf(d.x) < bounds.x + tentacle_touch && fabsf(d.y) < bounds.y + tentacle_touch && fabsf(d.z) < bounds.z + tentacle_touch;
}

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
        if (!zox_valid(user) || zox_has(user, Dead) || zox_has(user, SkillUsing) || !skill_pay_cost(world, e, user)) {
            continue;
        }
        const float3 position = zox_getv(user, Position3D);
        float3 ahead = zox_has(user, RaycastAim) ?
            float3_subtract(zox_getv(user, RaycastAim), position) :
            quaternion_to_normal(zox_getv(user, Rotation3D));
        const float3 direction = float3_normalize(ahead);
        const float length = tentacle_clear_length(world, zox_get_link(world, user, TerrainLink), position, direction,
            float_min(float3_length(ahead) + 0.3f, range->value));
        const float3 tip = float3_add(position, float3_scale(direction, length));
        skill_using_begin(world, user, e, tip);
        const color c = zox_has(e, Color) ? zox_getv(e, Color) : (color) { 110, 210, 120, 255 };
        for (int j = 0; j < tentacle_bits; j++) {
            // dark at its root, light at its tip
            const float u = (float) (j / tentacle_strands) / (tentacle_segments - 1);
            const float shade = 0.55f + 0.65f * u;
            const color shaded = { (byte) float_min(255, c.r * shade), (byte) float_min(255, c.g * shade), (byte) float_min(255, c.b * shade), c.a };
            const entity bit = spawn_particle3(world, position, float3_zero, shaded);
            zox_setv(bit, Gravity3D, float3_zero);
            zox_remove(bit, FadeOutTime);   // no fade, the render's slight see through only
            zox_setv(bit, Scale1, tentacle_thick - (tentacle_thick - tentacle_thin) * u);
            zox_setv(bit, DestroyInTime, tentacle_time + 0.05f);
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

void tentacle_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillUsing);
    zox_sys_in(SkillUsedAt);
    zox_sys_in(SkillTarget);
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkillUsing, skill);
        zox_sys_i(SkillUsedAt, start);
        zox_sys_i(SkillTarget, tip);
        zox_sys_i(Position3D, position);
        if (!zox_valid(skill->value) || !zox_has(skill->value, Tentacle)) {
            continue;
        }
        const float t = zox_current_time - start->value;
        const float extent = tentacle_extent(t);
        const float3 from = position->value;
        const float3 along = float3_subtract(tip->value, from);
        const float3 ahead = float3_normalize(along);
        const float3 side = float3_normalize(float3_cross(along, float3_up));
        const float3 up = float3_cross(side, ahead);
        const float length = float3_length(along) * extent;
        const float back = t > tentacle_windup + tentacle_out + tentacle_hold ?
            (t - tentacle_windup - tentacle_out - tentacle_hold) / tentacle_back : 0;
        entity bits[tentacle_bits];
        const uint count = zox_get_children_by_id(world, e, bits, tentacle_bits, zox_id(Particle3D));
        // out and held, not hit yet: its particles touching its attacker hit them
        const entity target = zox_get_link(world, e, AttackerLink);
        const byte hits = !zox_has(e, SkillHit) && t >= tentacle_windup && !back && zox_valid(target) &&
            zox_has(target, Character3) && !zox_has(target, Dead);
        const float3 target_centre = hits ? zox_getv(target, Position3D) : float3_zero;
        const float3 target_bounds = hits ? zox_getv(target, Bounds3D) : float3_zero;
        byte touched = 0;
        for (uint j = 0; j < count; j++) {
            const int segment = j / tentacle_strands, strand = j % tentacle_strands;
            const float u = (float) segment / (tentacle_segments - 1);
            const float radius = tentacle_root * powf(1 - u, 1.3f) + 0.015f;
            const float twist = strand * 2.094f + u * 5 + t * 8;
            float3 p;
            if (t < tentacle_windup) {
                // a bulge swelling on its body, its strands swirling: the tell
                const float k = t / tentacle_windup;
                const float a = u * 9.42f + twist;
                const float r = (0.1f + 0.12f * k) * (1 - 0.5f * u);
                p = float3_add(from, (float3) { cosf(a) * r, 0.05f + u * 0.15f * k, sinf(a) * r });
            } else {
                // its centre: out along it, arched up, a wave running down it, its tip curling as it's drawn back
                const float d = u * length;
                const float wave = sinf(u * 6 - t * 22) * 0.18f * u;
                const float arch = sinf(u * 3.1416f) * 0.25f * length / (1 + length);
                const float curl = back * u * u * u * 1.5f;
                float3 centre = float3_add(from, float3_scale(ahead, d * cosf(curl)));
                centre = float3_add(centre, float3_scale(up, arch + d * sinf(curl) * 0.6f));
                centre = float3_add(centre, float3_scale(side, wave * extent));
                // its strands round it, twisting
                p = float3_add(centre, float3_add(float3_scale(side, cosf(twist) * radius), float3_scale(up, sinf(twist) * radius)));
            }
            zox_setv(bits[j], Position3D, p);
            touched |= hits && tentacle_touches(p, target_centre, target_bounds);
        }
        if (touched) {
            zox_add(e, SkillHit);
            const float damage = get_skill_damage(world, skill->value, e,
                zox_getv(skill->value, SkillDamage), zox_getv(skill->value, SkillDamageMax));
            damage_character(world, e, target, skill->value, damage, float3_normalize(along));
        }
        if (t >= tentacle_time) {
            skill_using_end(world, e);
        }
    }
    zox_sys_on_end();
} zoxd_system(tentacle_system);
