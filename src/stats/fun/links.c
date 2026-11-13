// Stops all of a users regen stats, ie when health reaches 0
void disable_user_regens(
    ecs* world,
    entity user)
{
    if (!zox_valid(user)) {
        return;
    }
    iter it = zox_children(world, user);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity stat = it.entities[i];
            if (zox_has(stat, StatRegen) &&
                !zox_has(stat, StatDisabled))
            {
                zox_add(stat, StatDisabled);
            }
        }
    }
}

// is a stat made from this realm stat: its prefab, or its prefab's prefab (a character prefab's stats are
//  realm stats' instances, an npc's copies of them are those stats' instances)
static inline byte is_realm_stat_of(ecs* world, entity stat, entity realm_stat) {
    entity prefab = zox_get_prefab(world, stat);
    for (int depth = 0; prefab && depth < 4; depth++) {
        if (prefab == realm_stat) {
            return 1;
        }
        prefab = zox_get_prefab(world, prefab);
    }
    return 0;
}

// a user's stat made from a realm stat (an npc's are a step further down, copies of its kind's)
entity get_user_stat_from_realm(
    ecs* world,
    entity user,
    entity realm_stat)
{
    if (!zox_valid(user) || !zox_valid(realm_stat)) {
        return 0;
    }
    iter it = zox_children(world, user);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity stat = it.entities[i];
            if (is_realm_stat_of(world, stat, realm_stat)) {
                ecs_iter_fini(&it);
                return stat;
            }
        }
    }
    return 0;
}

// Resolves a user stats link from its realm stats link
// - realm regen -> realm health, becomes user regen -> user health
entity get_user_stat_link(
    ecs* world,
    entity user,
    entity stat)
{
    entity realm_stat = ecs_get_target(world, stat, EcsIsA, 0);
    if (!zox_valid(realm_stat)) {
        return 0;
    }
    entity realm_target = zox_get_link(world, realm_stat, StatLink);
    return get_user_stat_from_realm(world, user, realm_target);
}
