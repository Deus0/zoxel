// Shared skill damage, used by melee and projectiles

#define skill_damage_npc_multiplier 0.7f    // EASY MODE, npcs deal less damage
#define skill_damage_knockback_min 1.5f
#define skill_damage_knockback_max 3.5f
#define skill_damage_popup_spawn_y 0.18f

// Rolls a skills damage, adding the users damage stat, i.e. physical damage (boosted by strength)
float get_skill_damage(
    ecs* world,
    entity skill,
    entity user,
    float damage_min,
    float damage_max)
{
    float damage = randf_range(damage_min, damage_max);
    entity realm_damage_stat = zox_get_link(world, skill, SkillDamageStat);
    entity damage_stat = get_user_stat_from_realm(world, user, realm_damage_stat);
    if (zox_valid(damage_stat)) {
        damage += zox_getv(damage_stat, StatValue);
    }
    if (!zox_has(user, PlayerCharacter)) {
        damage *= skill_damage_npc_multiplier;
    }
    return damage;
}

// Damages a character, with defence, hit sound, knockback and a damage popup
//  - skill is used for the defence stat, it can be 0 (i.e. a projectile whose skill is gone)
//  - returns 0 if the character couldn't be damaged
byte damage_character(
    ecs* world,
    entity user,
    entity hit,
    entity skill,
    float damage,
    float3 knockback_normal)
{
    byte dbg_log = 0;
    color popup_color = (color) { 255, 0, 0, 255 };
    // targets defence, ie physical defence (boosted by endurance)
    entity realm_damage_stat = zox_valid(skill) ?
        zox_get_link(world, skill, SkillDamageStat) :
        0;
    entity realm_defence_stat = zox_valid(realm_damage_stat) ?
        zox_get_link(world, realm_damage_stat, StatDefenceLink) :
        0;
    entity defence_stat = get_user_stat_from_realm(world, hit, realm_defence_stat);
    if (zox_valid(defence_stat)) {
        damage -= zox_getv(defence_stat, StatValue);
        if (damage < 0) {
            damage = 0;
        }
    }
    entity health = zox_get_child_by_id(world, hit, zox_id(StatHealth));
    if (!zox_valid(health)) {
        zox_loge("hit user had no health");
        return 0;
    }
    float stat_value_max = zox_getv(health, StatValueMax);
    zox_muter(health, StatValue, state);
    state->value -= damage;
    if (state->value < 0) {
        state->value = 0;
    } else if (state->value > stat_value_max) {
        state->value = stat_value_max - damage;
        zox_loge("Stat is over max? [%f]", stat_value_max);
    }
    zox_add(health, Dirty);
    zox_add(health, DataDirty);
    if (dbg_log) {
        zox_log("[%s] took [%f] damage and is on [%f] health",
            zox_get_name(hit),
            damage,
            state->value)
    }
    if (zox_valid(user)) {
        combat_on_hit(world, hit, user);
    }
    zox_add(hit, JustHit);
    // hit sound
    spawn_character_hit_sound(world, hit);
    // knockback
    float3 hit_impulse = float3_scale(
        knockback_normal,
        randf_range(skill_damage_knockback_min, skill_damage_knockback_max));
    zox_muter(hit, Velocity3D, hit_velocity);
    hit_velocity->value = float3_add(hit_velocity->value, hit_impulse);
    // damage popup
    zox_geter_value(hit, Bounds3D, float3, bounds3D);
    zox_geter_value(hit, Position3D, float3, hit_character_position);
    float3 popup_position = (float3) {
        hit_character_position.x,
        hit_character_position.y + bounds3D.y + skill_damage_popup_spawn_y,
        hit_character_position.z
    };
    char popup_text[64];
    sprintf(popup_text, "%i", (int) floor(damage));
    spawn_popup3_easy(
        world,
        popup_text,
        popup_color,
        popup_position,
        zox_popup_scale,
        randf_range(4, 8));
    return 1;
}

// Queues damage on a terrain block, applied by block_damage_queue_system
void damage_terrain_block(
    ecs* world,
    entity chunk,
    byte3 position,
    float damage)
{
    if (!zox_valid(chunk) || !zox_has(chunk, BlockDamageQueue)) {
        return;
    }
    BlockDamageUpdate update = {
        .position = position,
        .damage = damage
    };
    zox_muter(chunk, BlockDamageQueue, damage_queue);
    a_BlockDamageQueue(damage_queue, update);
}
