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
        // a quest just given has its realm quest (IsA, its name) before its own link: that one's (it showed ???)
        if (!zox_valid(character)) {
            const entity from = ecs_get_target(world, quest, EcsIsA, 0);
            character = zox_valid(from) ? zox_get_link(world, from, CharacterLink) : 0;
        }
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
