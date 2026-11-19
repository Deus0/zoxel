// TODO: Move resource use out of this System
// NOTE: A melee skill used (Activate): its cost paid, its user's swing started; the damage lands at the swing's hit
//  (melee_hit, from the hit mark: eve/melee_hit.c), the windup is only animation
void melee_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillCost);
    zox_sys_in(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(SkillCost, cost);
        zox_sys_i(Activate, active);
        if (active->value != zox_dirty_active) {
            continue;
        }
        // user validation
        entity user = zox_get_parent(world, e);
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
        // validate their components
        if (!zox_has(user, RaycastVoxelData)) {
            zox_loge("User [%s]: Invalid Melee Components",
                zox_get_name(user));
            continue;
        }
        // does have resource
        // an npc's stats are copies of its kind's, a step further from the realm's (tst npc_attacks)
        const entity resource = get_user_stat_from_realm(world, user, zox_get_link(world, e, SkillResource));
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
                spawn_sound_effect(world, sound_effect_no_resource, user);
                continue;
            }
            // this should be muter -> instant use
            lresource = lresource - cost->value;
            zox_setv(resource, StatValue, lresource);
            if (dbg_log) {
                zox_log("User [%s] Skill Cost Subtracted [%f]",
                    zox_getn(user),
                    lresource);
            }
        }
        // the swing: its windup is animation, it hits at its hit mark (melee_hit, eve/melee_hit.c); its length the
        //  cooldown for now (doc/plans/skill_timing.md: a swing's own speed)
        skeleton_swing_begin(world, user, e, zox_current_time, zox_getv(e, CooldownTime), zox_swing_punch, float3_zero);
    }
    zox_sys_on_end();
} zoxd_system(melee_system);

// NOTE: A melee skill's hit, at its swing's hit mark: what the user aims at now takes the damage (a character, a
//  block), out of range or nothing a swing's sound
uint zox_melee_hits = 0;            // counted for tst punch_wind_up
double zox_melee_hit_time = 0;      // the last's

void melee_hit(ecs* world, entity e, entity user) {
    byte dbg_log = 0;
    zox_melee_hits++;
    zox_melee_hit_time = zox_current_time;
    if (!zox_valid(user) || zox_has(user, Dead) || !zox_has(user, RaycastVoxelData)) {
        return;
    }
    const RaycastVoxelData* raycast = zox_get(user, RaycastVoxelData);
    const SkillRange* range = zox_get(e, SkillRange);
    const SkillDamage* damage = zox_get(e, SkillDamage);
    const SkillDamageMax* damage_max = zox_get(e, SkillDamageMax);
    // Ray didn't hit anything
    entity hit = raycast->chunk;
    if (!zox_valid(hit)) {
        spawn_sound_effect(world, sound_effect_swing, user);
        if (dbg_log) {
            zox_logw("User [%s] Has no Raycast Target with range [%f:%f]",
                zox_get_name(user),
                range->value,
                zox_getv(user,
                RaycastRange));
        }
        return;
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
        spawn_sound_effect(world, sound_effect_out_of_range, user);
        if (dbg_log) {
            zox_logw("User [%s] [%f] is out of Range [%f]",
                zox_getn(user),
                raycast->distance,
                skill_range);
        }
        return;
    }
    float skill_damage = get_skill_damage(
        world,
        e,
        user,
        damage->value,
        damage_max->value);
    // Hitting NPC
    if (zox_has(hit, Character3)) {
        if (!damage_character(world, user, hit, e, skill_damage, raycast->normal)) {
            return;
        }
    }
    // Hitting Terrain
    else if (raycast->voxel && raycast->hit_block && zox_has(hit, TerrainChunk)) {
        entity block = raycast->hit_block;
        if (!zox_valid(block)) {
            zox_loge("TerrainChunk is valid but block is not.");
            return;
        }
        float block_damage = randf_range(1, 3);
        damage_terrain_block(world, raycast->chunk, raycast->positionl, block_damage);
        // Hit Bedrock!
        /*if (zox_has(block, BlockInvinsible)) {
            // cannot destroy voxel sound
            spawn_sound_effect(world, sound_effect_block_hard, user);
            return;
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
            return;
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
        spawn_sound_effect(world, sound_effect_block_hit, user);
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
