uint debug_label_chunk_link(ecs *world, entity player, char *buffer, uint size, uint index) {
    // index += snprintf(buffer + index, size - index, "characters [%lu]\n", zox_stats_characters);
    // index = get_debug_label_app(world, player, buffer, size, index);
    if (!player) {
        index += snprintf(buffer + index, size - index, "Invalid Player\n");
        return index;
    }
    zox_geter(player, CharacterLink, characterLink)
    if (!zox_valid(characterLink->value)) {
        index += snprintf(buffer + index, size - index, "[%s] has no Character\n", zox_get_name(player));
        return index;
    }
    return get_label_chunk_link(world, characterLink->value, buffer, size, index);
}
