// [none] Melee
extern entity spawn_pickup_block(ecs*, const float3, const entity);

// TODO: Check Resource Cost before warming up!
zox_sys2(MeleeSystem) {
    float popup_spawn_y = 0.18f;
    double volume = get_volume_sfx();
    color popup_color = (color) { 255, 0, 0, 255 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UserLink);
    zox_sys_in(SkillDamage);
    zox_sys_in(SkillDamageMax);
    zox_sys_in(SkillRange);
    zox_sys_in(SkillResourceLink);
    zox_sys_in(SkillCost);
    zox_sys_in(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(UserLink, userLink);
        zox_sys_i(SkillDamage, skillDamage);
        zox_sys_i(SkillDamageMax, skillDamageMax);
        zox_sys_i(SkillRange, range);
        zox_sys_i(SkillResourceLink, rresource);
        zox_sys_i(SkillCost, cost);
        zox_sys_i(Activate, active);

        if (active->value != zox_dirty_active) {
            continue;
        }

        // user validation
        entity user = userLink->value;
        if (!zox_valid(user) || zox_gett_value(user, Dead)) {
            continue;
        }

        // validate their components
        if (!zox_has(user, StatLinks) || !zox_has(user, RaycastVoxelData)) {
            zox_loge("User [%s]: Invalid Melee Components", zox_get_name(user));
            continue;
        }


        // does have resource
        entity resource = 0;
        entity strength = 0;

        zox_geter(user, RaycastVoxelData,  raycast);
        zox_geter(user, StatLinks, stats);

        for (int j = 0; j < stats->length; j++) {
            entity stat = stats->value[j];
            zox_get_prefab(stat, stat_parent);
            if (rresource->value == stat_parent) {
                resource = stat;
            }
            if (!strength && zox_has(stat, StatAttribute)) {
                strength = stat;
            }
        }

        // TODO: move cost use into activation system
        // resource cost
        if (!disable_skill_costs) {
            if (!zox_valid(resource) || !zox_has(resource, StatValue)) {
                // zox_logw("User [%s]: Invalid Skill Resource", zox_get_name(user));
                continue;
            }
            float lresource = zox_get_value(resource, StatValue);
            if (lresource < cost->value) {
                // zox_logw("User [%s]: Invalid Left [%f]", zox_get_name(user), lresource);
                continue;
            }
            // this should be muter -> instant use
            lresource = lresource - cost->value;
            zox_set(resource, StatValue, { lresource });

            // zox_log("User [%s] Skill Cost Subtracted [%f]", zox_get_name(user), lresource);
        }

        // Ray didn't hit anything
        entity hit = raycast->chunk;
        if (!zox_valid(hit)) {
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[44], 0.3, volume);
            // zox_logw("User [%s] Has no Ray Target", zox_get_name(user));
            continue;
        }

        float skill_range = range->value;
        byte in_range = debug_ray_big_range || !skill_range || raycast->distance <= skill_range;
        if (!in_range) {
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[47], 0.3, volume);
            // zox_logw("User [%s] [%f] is out of Range [%f]", zox_get_name(user), raycast->distance, skill_range);
            continue;
        }

        // todo: reduce energy stat value using SkillCost, check if has enough energy
        float skill_damage = randf_range(skillDamage->value, skillDamageMax->value);
        if (strength) {
            skill_damage += strength_damage_multiplier * zox_gett_value(strength, StatValue);
        }

        if (!zox_has(user, PlayerLink)) {
            skill_damage *= 0.6f; // EASY MODE
        }

        if (zox_has(hit, Character3)) {
            zox_geter(hit, StatLinks, hit_stats);
            find_array_element_with_tag(hit_stats, HealthStat, health_stat);
            if (!zox_valid(health_stat)) {
                zox_log_error("hit user had no health")
                continue;
            } else {
                float stat_value_max = zox_get_value(health_stat, StatValueMax);
                zox_muter(health_stat, StatValue, statValue);
                statValue->value -= skill_damage;
                if (statValue->value < 0) {
                    statValue->value = 0;
                } else if (statValue->value > stat_value_max) {
                    statValue->value = stat_value_max;
                }
                // zox_log("[%s] took [%f] damage and is on [%f] health", zox_get_name(hit), skill_damage, statValue->value)
                combat_on_hit(world, hit, user);
            }

            // hit sound
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[28], 0.6, volume);

            // add knockback
            float3 hit_impulse = float3_scale(raycast->normal, randf_range(knockback_min, knockback_max));
            zox_muter(hit, Velocity3D, hit_velocity);
            hit_velocity->value = float3_add(hit_velocity->value, hit_impulse);
            // zox_log("+ added impulse [%fx%fx%f]", hit_impulse.x, hit_impulse.y, hit_impulse.z)

            // damage popup
            zox_geter_value(hit, Bounds3D, float3, bounds3D);
            zox_geter_value(hit, Position3D, float3, hit_character_position);
            float3 popup_position = (float3) {
                hit_character_position.x, hit_character_position.y + bounds3D.y + popup_spawn_y,
                hit_character_position.z
            };
            char popup_text[64];
            sprintf(popup_text, "%i", (int) floor(skill_damage));
            spawn_popup3_easy(world, popup_text, popup_color, popup_position, 2.5f, randf_range(4, 8));

        } else if (raycast->voxel && raycast->hit_block && zox_has(hit, TerrainChunk)) {

            entity block = raycast->hit_block;
            if (!zox_valid(block)) {
                zox_log_error("TerrainChunk is valid but block is not.")
                continue;
            }

            if (!zox_has(block, BlockInvinsible)) {
                // effect our terrain here
                raycast_action(world, *raycast, 0, 2);
                // destroy voxel sound
                spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[24 + rand() % 6], 0.4, 1.2f * get_volume_sfx());

            } else {
                // cannot destroy voxel sound
                spawn_sound_generated(world, prefab_sound_generated,  instrument_violin, note_frequencies[42 + rand() % 6], 0.26, 1.4f * get_volume_sfx());
            }
        } else {
            // cannot hit air
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[44], 0.3, volume);
        }
    }
} zox_sys_end(MeleeSystem);
