// Spawns a projectile from raycast point
zox_sys2(ShootSystem) {
    byte dbg_log = 0;
    float default_power = 64;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillResourceLink);
    zox_sys_in(SkillCost);
    zox_sys_in(SkillDamage);
    zox_sys_in(SkillDamageMax);
    zox_sys_in(SkillRange);
    zox_sys_in(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(SkillResourceLink, rresource);
        zox_sys_i(SkillCost, cost);
        zox_sys_i(SkillDamage, skillDamage);
        zox_sys_i(SkillDamageMax, skillDamageMax);
        zox_sys_i(SkillRange, range);
        zox_sys_i(Activate, active);
        if (active->value != zox_dirty_active) {
            continue;
        }
        // user validation
        entity user = zox_get_parent(world, e);
        // entity user = userLink->value;
        if (!zox_valid(user)) {
            zox_loge("Skill has Invalid User [%s]", zox_get_name(e));
            continue;
        }
        if (zox_gett_value(user, Dead)) {
            if (dbg_log) {
                zox_logw("User [%s] is Dead, Cannot Attack.", zox_get_name(user));
            }
            continue;
        }
        zox_log("SHooting");
        float3 position;
        float4 rotation;
        if (zox_has(user, HandBoneLink)) {
            entity hand = zox_getv(user, HandBoneLink);
            if (!zox_valid(hand)) {
                continue;
            }
            position = zox_getv(hand, Position3D);
            rotation = zox_getv(hand, Rotation3D);
            // NOTE: Hand is facing down atm, just make go up a bit more
            rotation = quaternion_rotate(rotation, euler_to_quaternion((float3) { -90 * degrees_to_radians, 0, 0 }));
        } else if (zox_has(user, CameraLink)) {
            entity camera = zox_getv(user, CameraLink);
            if (!zox_valid(camera)) {
                continue;
            }
            position = zox_getv(camera, Position3D);
            rotation = zox_getv(camera, Rotation3D);
        } else {
            position = zox_getv(user, Position3D);
            rotation = zox_getv(user, Rotation3D);
        }
        spawn_projectile(world, prefab_projectile, position, rotation, 0.1f, default_power);
        // etc
    }
} zox_sys_end(ShootSystem);
