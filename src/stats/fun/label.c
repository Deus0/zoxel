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
    zox_geter(player, CharacterLink, characterLink);
    if (!zox_valid(characterLink->value)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }
    // zox_geter(characterLink->value, StatLinks, stats);
    // index = add_label_stat_level(world, characterLink->value, stats, StatSoul, buffer, size, index);
    /*for (int i = 0; i < stats->length; i++) {
        const entity stat = stats->value[i];
        if (!zox_valid(stat)) {
            continue;
        }
        if (zox_has(stat, StatState)) {
            index = add_label_stat_state(world, characterLink->value, stat, buffer, size, index);
        } else if (zox_has(stat, StatRegen)) {
            index = add_label_stat_value(world, characterLink->value, stat, buffer, size, index);
        } else if (zox_has(stat, StatAttribute)) {
            index = add_label_stat_value(world, characterLink->value, stat, buffer, size, index);
        }
    }*/

    /*zox_geter(characterLink->value, DotLinks, dots);
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
    zox_geter(player, CharacterLink, characterLink);
    if (!zox_valid(characterLink->value) || !zox_has(characterLink->value, ElementLinks)) {
        index += snprintf(buffer + index, size - index, "[%s] has invalid character\n", zox_get_name(player));
        return index;
    }
    zox_geter(characterLink->value, ElementLinks, elements);
    index += snprintf(buffer + index, size - index, "[%s]'s elements [%i]\n", zox_get_name(characterLink->value), elements->length);
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

/*int get_label_local_character_level(ecs *world, const entity character, char buffer[], int buffer_size, int buffer_index) {
 *    if (!character) return buffer_index;
 *    const StatLinks *stats = zox_get(character, StatLinks)
 *    find_array_element_with_tag(stats, StatSoul, soul_stat)
 *    if (!zox_has(soul_stat, StatValue)) return buffer_index;
 *    float level = zox_getv(soul_stat, StatValue)
 *    float experience_value = zox_getv(soul_stat, ExperienceValue)
 *    float experience_max = zox_getv(soul_stat, ExperienceMax)
 *    buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "lvl %i [%i/%i]\n", (int) level, (int) experience_value, (int) experience_max);
 *    return buffer_index;
 * }
 *
 *
 * int get_label_children(ecs *world, const entity e, char buffer[], int buffer_size, int buffer_index) {
 *    if (!e || !zox_has(e, Children)) return buffer_index;
 *    const Children *children = zox_get(e, Children)
 *    buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "[%s]'s children [%i]\n", zox_get_name(e), children->length);
 *    for (int i = 0; i < children->length; i++) {
 *        buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "  [%i] %s\n", i, zox_get_name(children->value[i]));
 *    }
 *    return buffer_index;
 * }
 *
 * int get_label_local_character_health(ecs *world, const entity character, char buffer[], int buffer_size, int buffer_index) {
 *    if (character) {
 *        const StatLinks *statLinks = zox_get(character, StatLinks)
 *        find_array_element_with_tag(statLinks, StatHealth, health_stat)
 *
 *        if (!zox_has(health_stat, StatValue)) return buffer_index;
 *
 *        float health_value = zox_getv(health_stat, StatValue)
 *        float health_value_max = zox_getv(health_stat, StatValueMax)
 *
 *        //if (!zox_has(meta_stat_health, ZoxName)) zox_log(" ! meta_stat_health has no ZoxName\n")
 *        //if (!zox_has(health_stat, ZoxName)) zox_log(" ! health_stat has no ZoxName\n")
 *        if (!zox_has(health_stat, ZoxName)) return buffer_index;
 *
 *        const ZoxName *health_name = zox_get(health_stat, ZoxName)
 *        char *name_string = convert_zext_to_text(health_name->value, health_name->length);
 *        // snprintf(text, hierarchy_max_line_characters, "%s", health_name_string);
 *        buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, " %s [%i/%i] ", name_string, (int) health_value, (int) health_value_max);
 *        free(name_string);
 *    }
 *    return buffer_index;
 * }*/
