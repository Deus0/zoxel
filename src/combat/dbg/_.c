void zox_tst_player_character_death(ecs *world, ClickEventData data) {
    entity player = dbg_player;
    zox_geter(player, CharacterLink, character);
    if (!zox_valid(character->value)) {
        zox_log("No player character to kill.");
        return;
    }
    zox_log("=> Testing Player Deaths");
    zox_log("   -> Hopefully they respawn.");
    zox_log("- killing player character [%s]", zox_get_name(character->value));
    // needs this so camera is removed
    zox_set(character->value, Dead, { zox_dirty_trigger });
    // zox_delete(character->value);
    /* else {
        zox_log("+ spawning player character");
        float3 spawned;
        game_start_player_new(world, player, &spawned);
    }*/
}
