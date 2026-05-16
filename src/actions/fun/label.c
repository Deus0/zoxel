/*uint get_label_player_actions(ecs *world, entity player, char *buffer, uint size, uint index) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }
    zox_geter(player, CharacterLink, characterLink)
    if (!zox_valid(characterLink->value)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }
    zox_geter(characterLink->value, ActionLinks, actions)
    index += snprintf(buffer + index, size - index, "[%s] has [%i] actions\n", zox_get_name(player), actions->length);
    for (int i = 0; i < actions->length; i++) {
        const entity action = actions->value[i];
        if (!zox_valid(action)) {
            continue;
        }
        index += snprintf(buffer + index, size - index, " - [%i] %s\n", i, zox_get_name(action));
    }
    return index;
}*/
