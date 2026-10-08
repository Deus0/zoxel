const char* get_quest_tracker_text(
    ecs* world,
    entity quest)
{
    if (!zox_valid(quest)) {
        return "";
    }
    const char* name =
        zox_has(quest, ZoxName) ?
        zox_getv(quest, ZoxName) :
        zox_getn(quest);
    char* text = malloc(512);
    if (!text) {
        return "";
    }
    int index = sprintf(text, "%s", name);
    if (zox_has(quest, SlayQuest)) {
        entity character = zox_get_link(world, quest, CharacterLink);
        const char* character_name =
            zox_valid(character) ?
            (zox_has(character, ZoxName) ?
            zox_getv(character, ZoxName) :
            zox_getn(character)) :
            "???";
        byte value = zox_getv(quest, QuestValue);
        byte target = zox_getv(quest, QuestTarget);
        index += sprintf(
            text + index,
            "\n- Slay [%s] [%i/%i]",
            character_name,
            value,
            target);
    } else {
        index += sprintf(
            text + index,
            "\n- Find [???]");
    }
    return text;
}
