extern entity game_start_player_new(ecs*, entity);

void toggle_player_death(ecs *world, int32_t keycode) {
    if (keycode != zox_key_j) {
        return;
    }
    zox_log("=> Testing Player Deaths");
    zox_log("   -> Hopefully they respawn.");

    entity player = dbg_player;
    zox_geter(player, CharacterLink, characterLink);
    if (zox_valid(characterLink->value)) {
        zox_log("- killing player character");
        zox_delete(characterLink->value);
    } else {
        zox_log("+ spawning player character");
        game_start_player_new(world, player);
    }
}
