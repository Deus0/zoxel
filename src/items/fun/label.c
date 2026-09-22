uint get_label_player_items(
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
    if (!zox_valid(character)) {
        index += snprintf(
            buffer + index,
            size - index,
            "[%s] has no character\n",
            zox_getn(player));
        return index;
    }
    index += snprintf(
        buffer + index,
        size - index,
        "[%s] has items\n",
        zox_getn(character));
    iter it2 = zox_children(world, character);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity item = it2.entities[j];
            if (!zox_has(item, Item)) {
                continue;
            }
            index += snprintf(
                buffer + index,
                size - index,
                " - [%s]\n",
                zox_getn(item));
        }
    }
    return index;
}
