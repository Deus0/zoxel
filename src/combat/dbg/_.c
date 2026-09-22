void zox_tst_player_character_death(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        zox_log("No player character to kill.");
        return;
    }
    zox_log("=> Testing Player Deaths");
    zox_log("   -> Hopefully they respawn.");
    zox_log("- killing player character [%s]",
            zox_getn(character));
    zox_add(character, PreDeath);
    /*zox_add(character->value, Dead);
    zox_add(character->value, DeathDirty);
    zox_setv(character->value, DiedTime, zox_current_time);*/
    // needs this so camera is removed
    // zox_set(character->value, Dead, { zox_dirty_trigger });
    // zox_delete(character->value);
    /* else {
        zox_log("+ spawning player character");
        float3 spawned;
        game_start_player_new(world, player, &spawned);
    }*/
}
