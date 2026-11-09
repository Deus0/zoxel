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

// Finds the users stat that was spawned from a realm stat
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
            if (ecs_has_pair(world, stat, EcsIsA, realm_stat)) {
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
