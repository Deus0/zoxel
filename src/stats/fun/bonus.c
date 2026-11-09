// Applies an attributes bonus to the users stats that scale from it
//  - realm stat links StatScaleLink to realm attribute, with StatScaleAmount per point
//  - StatBonus stores the bonus applied, so only the difference is added
//  - state stats (health) increase max and value, others (regen, base) increase value
void apply_attribute_bonus(
    ecs* world,
    entity attribute)
{
    byte dbg_log = 0;
    if (!zox_valid(attribute) ||
        !zox_has(attribute, StatValue))
    {
        return;
    }
    entity user = zox_get_parent(world, attribute);
    entity realm_attribute = ecs_get_target(world, attribute, EcsIsA, 0);
    if (!zox_valid(user) ||
        !zox_valid(realm_attribute))
    {
        return;
    }
    const float attribute_value = zox_getv(attribute, StatValue);
    iter it = zox_children(world, user);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity stat = it.entities[i];
            if (!zox_has(stat, Stat) ||
                !zox_has(stat, StatValue))
            {
                continue;
            }
            entity realm_stat = ecs_get_target(world, stat, EcsIsA, 0);
            if (!zox_valid(realm_stat) ||
                zox_get_link(world, realm_stat, StatScaleLink) != realm_attribute ||
                !zox_has(realm_stat, StatScaleAmount))
            {
                continue;
            }
            const float scale = zox_getv(realm_stat, StatScaleAmount);
            const float new_bonus = attribute_value * scale;
            const float old_bonus = zox_has(stat, StatBonus) ? zox_getv(stat, StatBonus) : 0;
            const float delta = new_bonus - old_bonus;
            if (delta == 0) {
                continue;
            }
            StatValue* value = zox_mut(stat, StatValue);
            if (zox_has(stat, StatState) &&
                zox_has(stat, StatValueMax))
            {
                StatValueMax* max = zox_mut(stat, StatValueMax);
                max->value += delta;
                value->value += delta;
                if (value->value > max->value) {
                    value->value = max->value;
                }
                // NOTE: dont kill, health at 0 is death
                if (value->value < 1 && old_bonus > new_bonus) {
                    value->value = 1;
                }
            } else {
                value->value += delta;
            }
            zox_setv(stat, StatBonus, new_bonus);
            zox_add(stat, Dirty);
            zox_add(stat, DataDirty);
            if (dbg_log) {
                zox_log("[%s] [%s] boosts [%s] by [%f] -> [%f]",
                    zox_getn(user),
                    zox_getn(attribute),
                    zox_getn(stat),
                    delta,
                    value->value);
            }
        }
    }
}

// Attribute changed, ie stat point spent
void attribute_bonus_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        apply_attribute_bonus(world, e);
    }
}
