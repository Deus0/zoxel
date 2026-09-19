// TODO: Check Resource Cost before warming up!
// TODO: Move resource use out of this System
// NOTE: Applies damage to character or terrain
void melee_system(iter* it) {
    byte dbg_log = 0;
    float npc_nerf_multiplier = 0.7f;
    color popup_color = (color) { 255, 0, 0, 255 };
    float popup_spawn_y = 0.18f;
    double volume = get_volume_sfx();
    float knockback_min = 1.5f;
    float knockback_max = 3.5f;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillCost);
    zox_sys_in(SkillDamage);
    zox_sys_in(SkillDamageMax);
    zox_sys_in(SkillRange);
    zox_sys_in(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(SkillCost, cost);
        zox_sys_i(SkillDamage, damage);
        zox_sys_i(SkillDamageMax, damage_max);
        zox_sys_i(SkillRange, range);
        zox_sys_i(Activate, active);
        if (active->value != zox_dirty_active) {
            continue;
        }
        // user validation
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user)) {
            zox_loge("Skill has Invalid User [%s]", zox_get_name(e));
            continue;
        }
        if (zox_has(user, Dead)) {
            if (dbg_log) {
                zox_logw("User [%s] is Dead, Cannot Attack.",
                    zox_get_name(user));
            }
            continue;
        }
        // validate their components
        if (!zox_has(user, RaycastVoxelData)) {
            zox_loge("User [%s]: Invalid Melee Components",
                zox_get_name(user));
            continue;
        }
        // does have resource
        entity resource = 0;
        // entity strength = 0;
        zox_geter(user, RaycastVoxelData, raycast);
        entity boost_stat = 0;
        entity rresource = zox_get_link(world, e, SkillResource);
        iter it2 = zox_children(world, user);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity stat = it2.entities[j];
                if (!zox_has(stat, Stat)) {
                    continue;
                }
                entity meta = zox_get_prefab(world, stat);
                if (rresource == meta) {
                    resource = stat;
                }
                // Assuming strength is first attribute
                if (!boost_stat && zox_has(stat, StatAttribute)) {
                    boost_stat = stat;
                }
            }
        }
        // entity user_stats[stats_children_capacity];
        // uint user_stats_length = zox_get_children(world, user, user_stats, stats_children_capacity);
        // for (uint j = 0; j < user_stats_length; j++) {
            // entity stat = user_stats[j];
        // TODO: move cost use into activation system
        // resource cost
        if (!disable_skill_costs) {
            if (!zox_valid(resource) ||
                !zox_has(resource, StatValue)) {
                if (dbg_log) {
                    zox_log("User [%s] has Invalid Skill Resource", zox_get_name(user));
                }
                continue;
            }
            float lresource = zox_getv(resource, StatValue);
            if (lresource < cost->value) {
                if (dbg_log) {
                    zox_log("User [%s] needs more [%s] [%f]",
                        zox_get_name(user),
                        zox_get_name(resource),
                        lresource);
                }
                spawn_sound_generated(
                    world,
                    prefab_sound_generated,
                    instrument_piano,
                    note_frequencies[14],
                    0.6,
                    0.6f * get_volume_sfx());
                continue;
            }
            // this should be muter -> instant use
            lresource = lresource - cost->value;
            zox_set(resource, StatValue, { lresource });
            if (dbg_log) {
                zox_log("User [%s] Skill Cost Subtracted [%f]", zox_get_name(user), lresource);
            }
        }
        // Temporary for now place here
        // TODO: Start swinging on Warmup
        if (zox_has(user, SwingStart)) {
            float swing_time = zox_getv(e, WarmupTime) + zox_getv(e, CooldownTime);
            zox_set(user, SwingStart, { zox_current_time });
            zox_set(user, SwingSpeed, { swing_time });
        }
        // Ray didn't hit anything
        entity hit = raycast->chunk;
        if (!zox_valid(hit)) {
            spawn_sound_generated(
                world,
                prefab_sound_generated,
                instrument_violin,
                note_frequencies[44],
                0.3,
                volume);
            if (dbg_log) {
                zox_logw("User [%s] Has no Raycast Target with range [%f:%f]",
                    zox_get_name(user),
                    range->value,
                    zox_getv(user,
                    RaycastRange));
            }
            continue;
        }
        float skill_range = range->value;
        if (!skill_range) {
            zox_loge("User [%s]'s Skill [%s] Range is 0",
                zox_getn(user),
                zox_getn(e));
        }
        byte in_range =
            debug_ray_big_range ||
            !skill_range ||
            raycast->distance <= skill_range;
        if (!in_range) {
            spawn_sound_generated(world, prefab_sound_generated, instrument_violin, note_frequencies[47], 0.3, volume);
            if (dbg_log) {
                zox_logw("User [%s] [%f] is out of Range [%f]",
                    zox_get_name(user),
                    raycast->distance,
                    skill_range);
            }
            continue;
        }
        // todo: reduce energy stat value using SkillCost, check if has enough energy
        float skill_damage = randf_range(damage->value, damage_max->value);
        if (boost_stat) {
            skill_damage += strength_damage_multiplier * zox_getv(boost_stat, StatValue);
        }
        if (!zox_has(user, PlayerCharacter)) {
            skill_damage *= npc_nerf_multiplier; // EASY MODE
        }
        // Hitting NPC
        if (zox_has(hit, Character3)) {
            entity hit_health = zox_get_child_by_id(world, hit, zox_id(StatHealth));
            if (!zox_valid(hit_health)) {
                zox_loge("hit user had no health")
                continue;
            } else {
                float stat_value_max = zox_getv(hit_health, StatValueMax);
                zox_muter(hit_health, StatValue, statValue);
                statValue->value -= skill_damage;
                if (statValue->value < 0) {
                    statValue->value = 0;
                } else if (statValue->value > stat_value_max) {
                    statValue->value = stat_value_max;
                }
                if (dbg_log) {
                    zox_log("[%s] took [%f] damage and is on [%f] health", zox_get_name(hit), skill_damage, statValue->value)
                }
                combat_on_hit(
                    world,
                    hit,
                    user);
            }
            // hit sound
            spawn_sound_generated(
                world,
                prefab_sound_generated,
                instrument_violin,
                note_frequencies[28],
                0.6,
                volume);
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
            spawn_popup3_easy(
                world,
                popup_text,
                popup_color,
                popup_position,
                zox_popup_scale,
                randf_range(4, 8));

        }
        // Hitting Terrain
        else if (raycast->voxel && raycast->hit_block && zox_has(hit, TerrainChunk)) {
            entity block = raycast->hit_block;
            if (!zox_valid(block)) {
                zox_loge("TerrainChunk is valid but block is not.");
                continue;
            }
            float block_damage = randf_range(1, 3);
            BlockDamageUpdate update = {
                // .meta = block,
                .position = raycast->positionl,
                .damage = block_damage
            };
            zox_muter(raycast->chunk, BlockDamageQueue, damage_queue);
            a_BlockDamageQueue(damage_queue, update);
            // Hit Bedrock!
            /*if (zox_has(block, BlockInvinsible)) {
                // cannot destroy voxel sound
                spawn_sound_generated(world, prefab_sound_generated,  instrument_violin, note_frequencies[42 + rand() % 6], 0.26, 1.4f * get_volume_sfx());
                continue;
            }
            // effect our terrain here
            byte3 positionl = raycast->positionl;
            entity chunk = raycast->chunk;
            // First check Vode:
            zox_mut_begin(chunk, VoxelNode, root);
            VoxelNode* leaf = open_VoxelNode(root, raycast->depth, positionl, 0); //  getm
            // TODO: Subdivide Octree if not lowest level! we need it at lowest level for destruction
            if (!leaf) {
                zox_loge("Leaf is null in melee system");
                continue;
            }
            byte linked = is_linked_VoxelNode(leaf);
            entity world_block;
            if (linked) {
                world_block = get_entity_VoxelNode(leaf);
            }
            // TODO: Refactor this damage into a damage system against blocks
            // Create new block health
            float block_health;
            if (!zox_valid(world_block) || !zox_has(world_block, StatValue)) {
                float2 health_start_range = zox_has(block, BlockHealth) ? zox_getv(block, BlockHealth) : (float2) { 1, 1 };
                block_health = randf_range(health_start_range.x, health_start_range.y);
            } else {
                block_health = zox_getv(world_block, StatValue);
            }
            block_health -= block_damage;
            if (block_health <= 0) {
                // Only dispose block if had to spawn health entity
                if (zox_valid(world_block)) {
                    zox_delete(world_block);
                    dispose_node_link_VoxelNode(leaf);
                    zox_mut_end(chunk, VoxelNode);
                    if (dbg_log_block) {
                        zox_log("Disposing of Block");
                    }
                }
                // finally remove from chunk
                zox_muter(chunk, VoxelNodeQueue, queue);
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = 0, .pos = positionl });
            } else {
                if (!zox_valid(world_block)) {
                    // create health entity
                    // TODO: Get health off meta or use prefab
                    world_block = zox_new();
                    zox_set_unique_name(world_block, "block_health");
                    link_node_VoxelNode(leaf, world_block);
                    zox_mut_end(chunk, VoxelNode);
                    zox_set(world_block, StatValue, { block_health });
                    if (dbg_log_block) {
                        zox_log("- New Block Health [%s]:%f", zox_get_name(world_block), block_health);
                    }
                } else if (!zox_has(world_block, StatValue)) {
                    zox_set(world_block, StatValue, { block_health });
                    if (dbg_log_block) {
                        zox_log("- Grass Block Health [%s]:%f", zox_get_name(world_block), block_health);
                    }
                } else {
                    zox_muter(world_block, StatValue, health);
                    health->value = block_health;
                    if (dbg_log_block) {
                        zox_log("- Block Health [%s]:%f", zox_get_name(world_block), block_health);
                    }
                }
            }
            // zox_muter(chunk, VoxelNodeQueue, queue);
            // a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = 0, .pos = positionl });
            // add health to block child
            // destroy voxel sound
            spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[24 + rand() % 6], 0.4, 1.2f * get_volume_sfx());
            // hit block popup
            float3 popup_position = raycast->positionf;
            popup_position.x += randf_range(-0.15f, 0.15f);
            popup_position.z += randf_range(-0.15f, 0.15f);
            popup_position.y += randf_range(0.2f, 0.33f);
            char popup_text[64];
            sprintf(popup_text, "%i", (int) floor(block_damage));
            spawn_popup3_easy(world, popup_text, popup_color, popup_position, 2.5f, randf_range(4, 8));
            if (dbg_log) {
                zox_log("User [%s] hit block at []", zox_get_name(user));
            }*/
        }
    }
    zox_sys_on_end();
} zoxd_system(melee_system);
