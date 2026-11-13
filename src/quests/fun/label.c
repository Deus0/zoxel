/*uint get_label_player_quests(
    ecs *world,
    const entity player,
    char *buffer,
    const uint size,
    uint index
) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(characterLink->value)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }

    const QuestLinks* quests = zox_get(characterLink->value, QuestLinks);
    index += snprintf(buffer + index, size - index, "[%s] has [%i] quests\n", zox_get_name(player), quests->length);
    for (int i = 0; i < quests->length; i++) {
        const entity action = quests->value[i];
        if (!zox_valid(action)) {
            continue;
        }
        index += snprintf(buffer + index, size - index, " - [%i] %s\n", i, zox_get_name(action));
    }

    return index;
}*/
