uint get_label_player_skills(
    ecs* world,
    entity player,
    char *buffer,
    uint size,
    uint index)
{
    if (!player) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }
    index += snprintf(
        buffer + index,
        size - index,
        "[%s] has [X] skills\n",
        zox_getn(character));
    iter it2 = zox_children(world, character);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity skill = it2.entities[j];
            if (!zox_has(skill, Skill)) {
                continue;
            }
            index += snprintf(
                buffer + index,
                size - index,
                " - [%s]\n",
                zox_getn(skill));
        }
    }
    return index;
}
