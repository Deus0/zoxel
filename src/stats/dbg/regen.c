// Cheat: increases all of the player characters regen stats by 1
static inline void zox_tst_regen_up(
    ecs* world,
    ClickEventData data)
{
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        return;
    }
    iter it = zox_children(world, character);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity stat = it.entities[i];
            if (!zox_has(stat, Stat) ||
                !zox_has(stat, StatRegen))
            {
                continue;
            }
            StatValue* value = zox_mut(stat, StatValue);
            value->value += 1;
            zox_add(stat, Dirty);
            zox_add(stat, DataDirty);
            zox_log("Regen Up (test) [%s] [%s] -> [%f]",
                zox_getn(character),
                zox_getn(stat),
                value->value);
        }
    }
}
