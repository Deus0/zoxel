// Spawns a projectile from raycast point
#define shoot_speed 10.0f      // a shot's speed a unit of its skill's range, a second
#define npc_shot_lift 0.04f    // of its distance, aimed above its target, its shot falls

void shoot_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(SkillRange);
    zox_sys_in(SkillDamage);
    zox_sys_in(SkillDamageMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(Activate, active);
        zox_sys_i(SkillRange, range);
        zox_sys_i(SkillDamage, damage_min);
        zox_sys_i(SkillDamageMax, damage_max);
        if (active->value != zox_dirty_active) {
            continue;
        }
        // user validation
        entity user = zox_get_parent(world, e);
        // entity user = userLink->value;
        if (!zox_valid(user)) {
            zox_loge("Skill has Invalid User [%s]",
                zox_get_name(e));
            continue;
        }
        if (zox_has(user, Dead)) {
            if (dbg_log) {
                zox_logw("User [%s] is Dead, Cannot Attack.",
                    zox_get_name(user));
            }
            continue;
        }
        // entity rresource = zox_get_link(world, e, SkillResource);
        float3 position;
        float4 rotation;
        entity skeleton = get_skeleton(world, user);
        entity hand = zox_valid(skeleton) ?
            zox_get_link(world, skeleton, HandBoneLink) :
            0;
        if (zox_valid(hand)) {
            position = zox_getv(hand, Position3D);
            rotation = zox_getv(hand, Rotation3D);
            // NOTE: Hand is facing down atm, just make go up a bit more
            float3 euler_add = (float3) {
                -90 * degrees_to_radians,
                0,
                0
            };
            rotation = quaternion_rotate(
                rotation,
                euler_to_quaternion(euler_add));
        } else {
            entity camera = zox_get_link(world, user, CameraLink);
            if (zox_valid(camera)) {
                position = zox_getv(camera, Position3D);
                rotation = zox_getv(camera, Rotation3D);
            } else {
                position = zox_getv(user, Position3D);
                rotation = zox_getv(user, Rotation3D);
            }
        }
        // one aiming (an npc at its threat, RaycastAim) shoots at it: where its attacker will be when the shot gets
        //  there (its velocity), a little above as its shot falls (npc_shot_lift)
        if (zox_has(user, RaycastAim)) {
            float3 aim = zox_getv(user, RaycastAim);
            const entity attacker = zox_get_link(world, user, AttackerLink);
            if (zox_valid(attacker) && zox_has(attacker, Velocity3D)) {
                const float flight = float3_length(float3_subtract(aim, position)) / (shoot_speed * range->value);
                float3 velocity = zox_getv(attacker, Velocity3D);
                velocity.y = 0;
                aim = float3_add(aim, float3_scale(velocity, flight));
            }
            float3 to_aim = float3_subtract(aim, position);
            to_aim.y += npc_shot_lift * float3_length(to_aim);
            rotation = quaternion_from_between_vectors((float3) { 0, 0, -1 }, float3_normalize(to_aim));
        }
        float damage = get_skill_damage(
            world,
            e,
            user,
            damage_min->value,
            damage_max->value);
        // projectile takes the skills color
        color projectile_color = zox_has(e, Color) ?
            zox_getv(e, Color) :
            color_white;
        spawn_projectile(
            world,
            prefab_projectile,
            user,
            e,
            position,
            rotation,
            0.1f,
            shoot_speed * range->value,
            damage,
            projectile_color);
    }
    zox_sys_on_end();
} zoxd_system(shoot_system);
