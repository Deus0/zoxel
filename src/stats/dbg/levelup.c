static inline void zox_tst_level_up(
    ecs* world,
    ClickEventData data)
{
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return;
    }
    entity soul = zox_get_child_by_id(world, character, zox_id(StatSoul));
    if (!zox_valid(soul)) {
        return;
    }
    float max = zox_getv(soul, ExperienceMax);
    zox_log("Giving [%s] Level Up (test) - [%f] Experience", zox_getn(character), max);
    ExperienceValue* value = zox_mut(soul, ExperienceValue);
    value->value += max;
}
