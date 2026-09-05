extern uint zox_dbg_ui_hierarchy(ecs *world, entity e, char *buffer, uint size, uint index, byte dig);

uint zox_dbg_ui_player_character(ecs *world, entity player, char *buffer, uint size, uint index) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "Invalid player\n");
        return index;
    }
    entity character = zox_get_link(world, player, Character);
    if (zox_valid(character)) {
        index += snprintf(buffer + index, size - index, "Player [%s] Canvas [%s]\n", zox_get_name(player), zox_get_name(character));
        return zox_dbg_ui_hierarchy(world, character, buffer, size, index, 0);
    } else {
        index += snprintf(buffer + index, size - index, "Invalid Player Character\n");
        return index;
    }
}
