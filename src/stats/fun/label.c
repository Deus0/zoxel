/*uint add_label_stat_level(
    ecs *world,
    entity character,
    const StatLinks *stats
    entity tag,
    char *buffer,
    uint size,
    uint index)
{
    find_array_element_with_tag_id(stats, tag, stat);
    if (!zox_valid(stat) || !zox_has(stat, StatValue)) {
        index += snprintf(buffer + index, size - index, "[%s] has no stat [%s]\n", zox_get_name(character), zox_get_name(tag));
        return index;
    }
    zox_geter(stat, ZoxName, name);
    float level = zox_getv(stat, StatValue);
    float experience_value = zox_getv(stat, ExperienceValue);
    float experience_max = zox_getv(stat, ExperienceMax);
    index += snprintf(buffer + index, size - index, "%s [lvl %i - %i/%i]\n", name->value, (int) level, (int) experience_value, (int) experience_max);
    return index;
}*/

uint add_label_stat_state(ecs *world, entity character, entity stat, char *buffer, uint size, uint index) {
    if (!zox_valid(stat) || !zox_has(stat, StatValue) || !zox_has(stat, ZoxName)) {
        index += snprintf(buffer + index, size - index, "[%s] has invalid state stat\n", zox_get_name(character));
        return index;
    }
    zox_geter(stat, ZoxName, name);
    float value = zox_getv(stat, StatValue);
    float value_max = zox_getv(stat, StatValueMax);
    index += snprintf(buffer + index, size - index, " - %s [%i/%i]\n", name->value, (int) value, (int) value_max);

    return index;
}

uint add_label_stat_value(ecs *world, entity character, entity stat, char *buffer, uint size, uint index) {
    if (!zox_valid(stat) || !zox_has(stat, StatValue) || !zox_has(stat, ZoxName)) {
        index += snprintf(buffer + index, size - index, "[%s] has invalid attribute stat\n", zox_get_name(character));
        return index;
    }
    zox_geter(stat, ZoxName, name);
    float value = zox_getv(stat, StatValue);
    index += snprintf(buffer + index, size - index, " - %s [%i]\n", name->value, (int) value);
    return index;
}

// new label ui
uint get_label_player_stats(
    ecs *world,
    entity player,
    char* buffer,
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
    // index = add_label_stat_level(world, character, stats, StatSoul, buffer, size, index);
    /*for (int i = 0; i < stats->length; i++) {
        const entity stat = stats->value[i];
        if (!zox_valid(stat)) {
            continue;
        }
        if (zox_has(stat, StatState)) {
            index = add_label_stat_state(world, character, stat, buffer, size, index);
        } else if (zox_has(stat, StatRegen)) {
            index = add_label_stat_value(world, character, stat, buffer, size, index);
        } else if (zox_has(stat, StatAttribute)) {
            index = add_label_stat_value(world, character, stat, buffer, size, index);
        }
    }*/

    /*zox_geter(character, DotLinks, dots);
    for (int i = 0; i < dots->length; i++) {
        const entity dot = dots->value[i];
        if (!zox_valid(dot)) {
            continue;
        }
        index += snprintf(buffer + index, size - index, " . %s\n", zox_get_name(dot));
    }*/
    return index;
}

uint get_label_player_element_links(ecs *world, const entity player, char *buffer, const uint size, uint index) {
    if (!zox_valid(player)) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character) || !zox_has(character, ElementLinks)) {
        index += snprintf(buffer + index, size - index, "[%s] has invalid character\n", zox_get_name(player));
        return index;
    }
    zox_geter(character, ElementLinks, elements);
    index += snprintf(buffer + index, size - index, "[%s]'s elements [%i]\n", zox_get_name(character), elements->length);
    for (int i = 0; i < elements->length; i++) {
        index += snprintf(buffer + index, size - index, " - [%i] %s\n", i, zox_get_name(elements->value[i]));
    }
    return index;
}

int debug_can_jump(ecs *world, entity character, char buffer[], int buffer_size, int buffer_index) {
    if (!character) {
        return buffer_index;
    }
    byte can_jump = zox_getv(character, Grounded);
    int is_jump = (int) 1000.0 * zox_getv(character, Jump);
    // buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, " grounded [%i] movement_disabled [%i]", can_jump, movement_disabled);
    buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, " grounded [%i] jump [%i ms]", can_jump, is_jump);
    return buffer_index;
}
